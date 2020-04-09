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

#pragma once

#include <vector>


/**
 * A Filter to convert from FFT Hz bins to Mel bins.
 *
 * Based on method mel(...) in
 *  https://github.com/librosa/librosa/blob/master/librosa/filters.py
 * "divide the triangular mel weights by the width of the mel band
 (area normalization)."
 */
class AMelFilter
{
public:
   AMelFilter(size_t numMels, size_t numFftBins, float sampleRate);
   
   /**
    * Equivalent to fWeights dot powers.
    * \param powers a list of kNumFftBins values to convert to kNumMels bins.
    * \param rowStride the number of elements per row in powers
    * \param out a buffer big enough for at least kNumMels floats.
    */
   void HzToMels(const float* powers, size_t rowStride, float* out) const;
   
   const std::vector<float>& Weights() const;
   
private:
   void InitWeights(float sampleRate, bool norm = true);
   std::vector<float> fWeights;
   const size_t fNumMels;
   const size_t fNumFftBins;
};
