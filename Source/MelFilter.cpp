/*
  ==============================================================================

   Copyright 2020 Art+Logic, Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

  ==============================================================================
*/

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iterator>
#include <numeric>
#include <cassert>

#include "../JuceLibraryCode/JuceHeader.h"

#include "MelFilter.h"


namespace
{
   /** \return  out[n] = data[n + 1] - data[n] for n in 1 to data.size(). */
   std::vector<float> Diff(std::vector<float>& data);

   /**
    * \param a The left hand operand, it can be a 1xN vector or MxN where N is equal
    *          to the length of v.
    * \param b The right hand operand, it must be a vector of length at least numCols.
    * \param outIt The output vector or matrix, the elements have to have been
    *              allocated.
    */
   template<class Iter, class Iter2, class OutputIt>
   void MatrixMultiply(Iter a, Iter aEnd, size_t numCols, const Iter2 b, size_t numBCols,
                   OutputIt outIt);

   /**
    * Perform op on each pair of values in left,right.
    */
   template <class T, class BinaryOp>
   std::vector<T> Outer(const std::vector<T>& left, const std::vector<T>& right, BinaryOp op);

   /** \param out On exit contains a sequence from min to max of evenly spaced values */
   void Linspace(std::vector<float>& out, float min, float max);

   /**
    * "Slaney" formula to convert hertz to mels
    * From https://github.com/librosa/librosa/blob/master/librosa/core/time_frequency.py
    */
   float HzToMel(float hz);
   float MelToHz(float mels);

   /**
    * \param minFreq minimum Hz of the sequence
    * \param maxFreq maximum Hz of the sequence
    * \return a sequence of lenght numBins from minFreq to maxFreq, calculated in
    *          the mel frequency space, but reutrned as Hz.
    */
   std::vector<float> MelFrequencies(size_t numBins, float minFreq, float maxFreq);

   /** \returns 1 + numBins / 2 values from 0 to sampleRate / 2 */
   std::vector<float> FftFrequencies(float sampleRate, size_t numBins);
}


AMelFilter::AMelFilter(size_t numMels, size_t numFftBins, float sampleRate)
: fNumMels(numMels)
, fNumFftBins(numFftBins)
{
   this->InitWeights(sampleRate);
}


void AMelFilter::HzToMels(const float* spectrum, size_t rowStride, float* out) const
{
   MatrixMultiply(fWeights.begin(), fWeights.end(), 1 + fNumFftBins / 2,
                  spectrum, rowStride, out);
}


void AMelFilter::InitWeights(float sampleRate, bool norm/*=true*/)
{
   using Bins = std::vector<float>;

   constexpr float minFreq = 0.0f;
   float maxFreq = sampleRate / 2.0f;

   constexpr size_t kMelsPad = 2UL;

   Bins fftFreqs(FftFrequencies(sampleRate, fNumFftBins));
   Bins melFreqs(MelFrequencies(fNumMels + kMelsPad, minFreq, maxFreq));
   Bins fdiff(Diff(melFreqs)); // returns melFreqs.size() - 1 elements == kNumMelBins + 1
   Bins ramps(Outer(melFreqs, fftFreqs, std::minus<>()));

   const size_t numColumns = fftFreqs.size();
   fWeights.resize(fNumMels * numColumns, 0.0f);

   for (size_t i = 0UL, w = 0UL; i < fNumMels; ++i)
   {
      size_t rampRow2Off = (i + kMelsPad) * numColumns;
      for(size_t j = 0UL; j < numColumns; ++j, ++w)
      {
         auto lower = -ramps[w] / fdiff[i];
         auto upper = ramps[rampRow2Off + j] / fdiff[i + 1];
         fWeights[w] = std::max(0.0f, std::min(lower, upper));
      }
   }

   if (norm)
   {
      // librosa: Slaney-style mel is scaled to be approx constant energy per channel
      // enorm = 2.0 / (mel_f[2:n_mels+2] - mel_f[:n_mels])
      Bins enorm(fNumMels);
      std::transform(melFreqs.begin() + kMelsPad, melFreqs.end(), melFreqs.begin(),
                     enorm.begin(), std::minus<>());
      std::transform(enorm.begin(), enorm.end(), enorm.begin(),
                     std::bind(std::divides<Bins::value_type>(),
                               2.0, std::placeholders::_1));

      // weights *= enorm[:, np.newaxis]
      auto weightIt = fWeights.begin();
      auto weightEnd = fWeights.end();
      auto rowNormIt = enorm.begin();
      while (weightEnd != weightIt)
      {
         auto endRow = weightIt + numColumns;
         float rowNorm = *rowNormIt++;
         while (weightIt != endRow)
         {
            *weightIt++ *= rowNorm;
         }
      }
   }
}


const std::vector<float>& AMelFilter::Weights() const
{
   return fWeights;
}


// MARK: - Utility Methods

namespace
{
   std::vector<float> Diff(std::vector<float>& data)
   {
      std::vector<float> adjDiff(data.size());
      std::adjacent_difference(data.begin(), data.end(), adjDiff.begin());
      // To match numpy.diff throw away first element which STL set to *data.begin().
      adjDiff.erase(adjDiff.begin());
      return adjDiff;
   }


   /** Copy a column of a matrix */
   template<class Iter, class OutIter>
   void Column(Iter in, size_t stride, OutIter outIt, size_t length)
   {
      for ( ; length > 0; (void)--length, in += stride)
         *outIt++ = *in;
   }

   // Actually matrix multiply, numpy does this when both dot arguments are 2D
   template<class Iter, class Iter2, class OutputIt>
   void MatrixMultiply(Iter a, Iter aEnd, size_t numACols, Iter2 b, size_t numBCols,
                   OutputIt outIt)
   {
      auto numARows = (aEnd - a) / numACols;
      // Validate numACols as being a reasonable value for the size of A.
      assert((numARows * numACols) == (aEnd - a));

      std::vector<float> bColumn(numACols);
      for (int i = 0; i < numBCols; ++i, (void)++outIt)
      {
         Column(b + i, numBCols, bColumn.begin(), bColumn.size());
         auto outColIt = outIt;
         auto aRowIt = a;

         for (int j = 0; j < numARows; ++j, outColIt += numBCols, aRowIt += numACols)
         {
            *outColIt = std::inner_product(aRowIt, aRowIt + numACols, bColumn.begin(), 0.0f);
         }
      }
   }


   std::vector<float> FftFrequencies(float sampleRate, size_t numBins)
   {
      std::vector<float> out(1UL + numBins / 2UL);
      Linspace(out, 0.0, sampleRate / 2.0);
      return out;
   }


   void Linspace(std::vector<float>& out, float min, float max)
   {
      struct EvenlySpaced
      {
         EvenlySpaced(float min, float max, size_t count)
         : fX(min), fStep((max - min) / (count - 1))
         {
            assert(count > 1);
         }
         float operator()() { auto x = fX; fX += fStep; return x; }
      private:
         float fX;
         const float fStep;
      };
      std::generate(out.begin(), out.end() - 1, EvenlySpaced(min, max, out.size()));
      out.back() = max;
   }


   float HzToMel(float hz)
   {
      constexpr float f_min = 0.0f;
      constexpr float f_sp = 200.0f / 3.0f;
      constexpr float min_log_hz = 1000.0f;

      float mel = (hz - f_min) / f_sp; // lineear
      if (hz >= min_log_hz)
      {
         constexpr float min_log_mel = (min_log_hz - f_min) / f_sp;
         constexpr float logstep = 0.06875177742094912f; // == ::logf(6.4f) / 27.0f;

         mel = min_log_mel + ::logf(hz / min_log_hz) / logstep;
      }
      return mel;
   }


   float MelToHz(float mels)
   {
      constexpr float f_min = 0.0f;
      constexpr float f_sp = 200.0f / 3.0f;
      constexpr float min_log_hz = 1000.0;
      constexpr float min_log_mel = (min_log_hz - f_min) / f_sp;

      float hz = f_min + f_sp * mels;
      if (mels >= min_log_mel)
      {
         constexpr float logstep = 0.06875177742094912f; // = np.log(6.4) / 27.0

         hz = min_log_hz * ::expf(logstep * (mels - min_log_mel));
      }
      return hz;
   }


   std::vector<float> MelFrequencies(size_t numBins, float minFreq, float maxFreq)
   {
      std::vector<float> out(numBins);
      Linspace(out, HzToMel(minFreq), HzToMel(maxFreq));
      std::transform(out.begin(), out.end(), out.begin(), MelToHz);
      return out;
   }


   /**
    * Emulate numpy.subtract.outer which does pair-wise differences of two
    * vectors or matrices. We will only support vectors (1xN) here.
    */
   template <class T, class BinaryOp>
   std::vector<T> Outer(const std::vector<T>& left, const std::vector<T>& right, BinaryOp op)
   {
      std::vector<T> result(left.size() * right.size(), 0.0);
      auto outIt = result.begin();
      for (const T& a : left)
      {
         for (const T& b : right)
         {
            *outIt++ = op(a, b);
         }
      }
      return result;
   }


#if UNIT_TESTS

// MARK: - Unit Tests

   class AMatrixMultiplyTest : public juce::UnitTest
   {
   public:
      AMatrixMultiplyTest() : juce::UnitTest("Dot product testing") {}
      void runTest() override
      {
         this->beginTest("test two scalars");
         auto a = { 3 };
         auto b = { 4 };
         std::vector<int> intResult(1UL);
         MatrixMultiply(a.begin(), a.end(), 1UL, b.begin(), 1UL, intResult.begin());
         this->expectEquals(intResult[0], 12);

         this->beginTest("test two vectors");
         std::vector<float> v = { 2.0f, 3.0f };
         std::vector<float> floatResult(1);
         // 1x2 . 2.1 = 1x1
         MatrixMultiply(v.begin(), v.end(), 2UL, v.begin(), 1UL, floatResult.begin());
         this->expectEquals(floatResult[0], 2.0f * 2.0f + 3.0f * 3.0f);

         this->beginTest("test matrix dot vector");
         std::vector<double> m = { 1.0, 2.0, 3.0, 4.0 };
         floatResult.resize(2, 0.0f);
         // 2x2 . 2x1 = 2x1
         MatrixMultiply(m.begin(), m.end(), 2UL, v.begin(), 1UL, floatResult.begin());
         this->expectEquals(floatResult[0], (float)(1.0 * 2.0 + 2.0 * 3.0));
         this->expectEquals(floatResult[1], (float)(3.0 * 2.0 + 4.0 * 3.0));

         this->beginTest("test matrix result");
         floatResult.resize(4, 0.0f);
         // 2x1 . 1x2 = 2x2
         MatrixMultiply(v.begin(), v.end(), 1UL, v.begin(), 2UL, floatResult.begin());
         this->expectEquals(floatResult[0], 2.0f * 2.0f);
         this->expectEquals(floatResult[1], 3.0f * 2.0f);
         this->expectEquals(floatResult[2], 2.0f * 3.0f);
         this->expectEquals(floatResult[3], 3.0f * 3.0f);
      }
   };


   class AHzConvertTest : public juce::UnitTest
   {
   public:
      AHzConvertTest() : juce::UnitTest("Hz conversion testing") {}
      void runTest() override
      {
         const std::array<float, 6> hzs = {
          60.f, 100.f, 1000.f, 12343.0f, 44010.0f, 96000.0f
         };
         const std::array<float, 6> mels = {
          0.899999976f, 1.5f, 15.0f, 51.5530777f, 70.0446396f, 81.3888016f
         };

         this->beginTest("to mels tests");
         auto expectedIt = mels.begin();
         std::for_each(hzs.begin(), hzs.end(), [&expectedIt, this](float hz)
          {
             this->expectWithinAbsoluteError(HzToMel(hz), *expectedIt++, 1e-5f);
          });

         this->beginTest("to Hz tests");
         expectedIt = hzs.begin();
         std::for_each(mels.begin(), mels.end(), [&expectedIt, this](float mels)
          {
             this->expectWithinAbsoluteError(MelToHz(mels), *expectedIt++, 5e-2f);
          });
      }
   };


   class ALinspaceTest : public juce::UnitTest
   {
   public:
      ALinspaceTest() : juce::UnitTest("Linspace testing") {}
      void runTest() override
      {
         this->beginTest("basic test");

         std::vector<float> out(10, 0.0f);
         Linspace(out, 0.0f, 100.0f);
         const std::array<float, 10> expected =
         {
            0.0, 11.11111111f, 22.22222222f, 33.33333333f, 44.44444444f,
                 55.55555556f, 66.66666667f, 77.77777778f, 88.88888889f, 100.0f
         };
         // totalErr = sum(out - expected)
         std::transform(out.begin(), out.end(), expected.begin(), out.begin(),
                        std::minus<float>());
         float totalErr = std::accumulate(out.begin(), out.end(), 0.0f);

         this->expectLessOrEqual(totalErr, 1e-4f * out.size());

         this->beginTest("mel frequency example test"); // $$$ not committed to this one
         out = MelFrequencies(40, 0.0f, 11025.0f);
         const std::array<float, 40> melExpected =
         {
            0.0f  ,     85.317f,    170.635f,   255.952f,
            341.269f,    426.586f,   511.904f,   597.221f,
            682.538f,    767.855f,   853.173f,   938.49f,
            1024.856f,   1119.114f,  1222.042f,  1334.436f,
            1457.167f,   1591.187f,  1737.532f,  1897.337f,
            2071.84f ,   2262.393f,  2470.47f ,  2697.686f,
            2945.799f,   3216.731f,  3512.582f,  3835.643f,
            4188.417f,   4573.636f,  4994.285f,  5453.621f,
            5955.205f,   6502.92f ,  7101.009f,  7754.107f,
            8467.272f,   9246.028f,  10096.408f, 11025.0f
         };

         this->expectEquals(out.size(), melExpected.size());

         std::transform(out.begin(), out.end(), melExpected.begin(), out.begin(),
                        std::minus<float>());
         totalErr = std::accumulate(out.begin(), out.end(), 0.0f);
         this->expectLessOrEqual(totalErr, 1e-4f * out.size());
      }
   };


   class AMelFilterTest : public juce::UnitTest
   {
   public:
      AMelFilterTest() : juce::UnitTest("MelFilter testing") {}
      void runTest() override
      {
         constexpr size_t numMelBins = 128UL;
         constexpr size_t numFFTBins = 2048UL;
         AMelFilter filter(numMelBins, numFFTBins, 22050.0f);

         this->beginTest("shape test");
         const size_t numColumns = (1UL + numFFTBins / 2UL);
         this->expectEquals(filter.Weights().size(), numMelBins * numColumns);

         // Expected values were taken from a Python session calling the librosa
         // equivalent to this class.
         const auto& weights = filter.Weights();
         std::array<std::array<float, 10>, 2> expectedRows = {{
            // row 0
            { 0.0f, 0.01618285f, 0.03236571f, 0.02899009f, 0.01280723f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f },
            // row 1
            { 0.0f, 0.0f, 0.0f, 0.00977924f, 0.02596209f, 0.03539371f, 0.01921085f, 0.003028f, 0.0f, 0.0f }
         }};
         this->beginTest("test sampling of beginning weights");
         {
            auto weightIt = weights.begin();
            for (const auto& expected : expectedRows)
            {
               for (size_t i = 0UL; i < expected.size(); ++i)
               {
                  this->expectWithinAbsoluteError(*(weightIt + i), expected[i], 1e-6f);
               }
               weightIt += numColumns;
            }
         }

         this->beginTest("test sampling of end weights");
         {
            expectedRows = {{
             // row 127
             { 0.00117234f, 0.00104208f, 0.00091182f, 0.00078156f, 0.0006513f,
               0.00052104f, 0.00039078f, 0.00026052f, 0.00013026f, 0.0f },
             // row 126
             { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }
            }};
            auto rweightIt = weights.rbegin();
            for (const auto& expected : expectedRows)
            {
               size_t expIdx = expected.size() - 1UL;
               assert(expIdx < expected.size() && "We have zero expected values somehow.");
               for (size_t i = 0UL; i < expected.size(); ++i)
               {
                  this->expectWithinAbsoluteError(*(rweightIt + i), expected[expIdx - i], 1e-6f);
               }
               rweightIt += numColumns;
            }
         }

         auto maxIt = std::max_element(weights.begin(), weights.end());
         auto maxVal = *maxIt;
         auto maxIdx = maxIt - weights.begin();
         this->expectWithinAbsoluteError(maxVal, 0.038422f, 1e-6f);
         this->expectEquals(maxIdx, 4112L);
      }
   };


   // Register tests by declaring them.
   static AMatrixMultiplyTest sMatrixMultiplyTest;
   static AHzConvertTest sHzConvertTest;
   static ALinspaceTest sLinspaceTest;
   static AMelFilterTest sMelFilterTest;

#endif
}
