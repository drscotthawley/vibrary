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
#include <cassert>
#include <cmath>
#include <stdexcept>
#include <tuple>
#include <vector>

#include "cnpy.h" // for numpy file support

#include "../JuceLibraryCode/JuceHeader.h"

#include "Float16.h"
#include "MelFilter.h"
#include "Spectrogram.h"

using juce::AudioData;
using juce::File;
using Spectrum = ASpectrogram::Spectrum;


namespace
{
   constexpr size_t kNumFftBins = 2048UL;
   constexpr size_t kHopLength = 512UL;
   
   // For scaling dB output
   constexpr float kMaxDb = 80.0f;
   
   struct Signal
   {
      AudioSampleBuffer buffer;
      double sampleRate;
   };

   /**
    * Convert signal's audio to mono by summing the channels.
    * \return signal if already mono or a new signal with a mono buffer.
    */
   Signal ToMono(const Signal& signal);
}


// MARK: -

/**
 * Helper to read audio files and produce the spectrogram.
 *
 * Many objects used to create the spectrogram are somewhat expensive to create.
 * This object caches them.
 */
class ASpectroMaker
{
public:
   /**
    * \param fftBins number of frequency bins for Hz based FFT.
    * \param melBins number of frequency bins for final Mel output.
    */
   ASpectroMaker(int fftBins, int melBins);

   void MakeFromFile(ASpectrogram& spectrogram, const File& file);
   
private:

   Signal ReadAudioSignal(const File& file, float lengthSeconds);
   ASpectrogram ShortTimeFourier(const AudioSampleBuffer& signal, size_t hop);
   
   static void AmplitudesToDbs(ASpectrogram& spectrum);
   static Spectrum HzToMels(const ASpectrogram& data, size_t channel, float sampleRate,
                            int numMelBins);

   AudioFormatManager fFormatMgr;
   dsp::FFT fFft;
   dsp::WindowingFunction<float> fWindow;
   const int fNumMelBins;
};


// MARK: - ASpectrogram Definition

std::unique_ptr<ASpectroMaker> ASpectrogram::sSpectroMaker = nullptr;
constexpr size_t ASpectrogram::kNumMelBins;


ASpectrogram::ASpectrogram(const std::string& audioFilename)
: fBasename(File(audioFilename).getFileName().toStdString())
, fRows(0UL)
, fColumns(0UL)
{
   if (!sSpectroMaker)
   {
      sSpectroMaker = std::make_unique<ASpectroMaker>(kNumFftBins, kNumMelBins);
      jassert(sSpectroMaker && "Failed to create ASpectroMaker.");
   }
   sSpectroMaker->MakeFromFile(*this, File(audioFilename));
}


ASpectrogram::ASpectrogram(const Spectra& spectra, size_t rows, size_t columns,
                           const std::string& basename)
: fBasename(basename)
, fSpectra(spectra)
, fRows(0UL)
, fColumns(0UL)
{
   this->Reshape(rows, columns); // this validates the shape
}


void ASpectrogram::CleanUp()
{
   sSpectroMaker.reset(nullptr);
}


void ASpectrogram::AddChannel(const Spectrum& spectrum)
{
   fSpectra.push_back(spectrum);
}


void ASpectrogram::AddChannel(const Spectrum&& spectrum)
{
   fSpectra.push_back(spectrum);
}


const Spectrum& ASpectrogram::Channel(size_t index) const
{
   return fSpectra.at(index);
}


Spectrum& ASpectrogram::Channel(size_t index)
{
   // Define in terms of const version.
   return const_cast<Spectrum&>(const_cast<const ASpectrogram*>(this)->Channel(index));
}


size_t ASpectrogram::ChannelCount() const
{
   return fSpectra.size();
}


std::string ASpectrogram::ConstructPath(const std::string& rootPath,
                                        const std::string& basename,
                                        const std::string& extension) const
{
   return File(rootPath).getChildFile(basename + extension)
    .getFullPathName().toStdString();
}


std::pair<float, float> ASpectrogram::MinMax() const
{
   size_t numChannels = this->ChannelCount();
   
   float minVal = std::numeric_limits<float>::max();
   float maxVal = std::numeric_limits<float>::min();
   
   for (size_t c = 0; c < numChannels; ++c)
   {
      auto& spectrum = this->Channel(c);
      
      auto [minIt, maxIt] = std::minmax_element(spectrum.begin(), spectrum.end());
      minVal = std::min(*minIt, minVal);
      maxVal = std::max(*maxIt, maxVal);
   }

   return { minVal, maxVal };
}


ASpectrogram ASpectrogram::FromNpz(const std::string& npzFilename)
{
   // npz_load returns map of array name to array data.
   cnpy::NpyArray array = cnpy::npz_load(npzFilename).begin()->second;
   
   // Obviously making assumptions here. The npz files we produce are [1,rows,columns, channels]
   size_t rows = 0UL, cols = 0UL;
   size_t channels = 1;
   switch (array.shape.size())
   {
      case 0:
      break;
         
      case 1:
      cols = array.shape[0];
      break;
         
      case 2:
      rows = array.shape[0];
      cols = array.shape[1];
      break;
      
      default:
      rows = array.shape[1];
      cols = array.shape[2];
      channels = array.shape[3];
      break;
   }
   
   // Deinterleave channels.
   Spectra spectra;
   std::vector<float> spectrum(array.num_vals);
   for (int c = 0; c < channels; ++c)
   {
      const Float16* src = array.data<Float16>() + c;
      auto dest = spectrum.begin();
      const auto end = spectrum.end();
      while (end != dest)
      {
         *dest++ = static_cast<float>(*src);
         src += channels;
      }
      spectra.push_back(std::move(spectrum));
   }
   return ASpectrogram(spectra,  rows, cols, "FromNpz");
}



void ASpectrogram::SetChannelSpectrum(size_t index, const Spectrum&& data)
{
   if (index == fSpectra.size())
   {
      this->AddChannel(data);
   }
   else if (index < fSpectra.size())
   {
      fSpectra[index] = data;
   }
   else
   {
      throw std::runtime_error("Can't set a spectrum two or more channels past "
                               "the number of existing channels.");
   }
}


void ASpectrogram::Reshape(size_t rows, size_t columns)
{
   if (this->ChannelCount() != 0
    && this->Channel(0).size() != (rows * columns))
   {
      throw std::runtime_error("New spectrogram shape does not have same size as channel data");
   }
   fRows = rows;
   fColumns = columns;
}


// MARK: - ASpectroMaker Definition

ASpectroMaker::ASpectroMaker(int fftBins, int melBins)
: fFft(std::log2(fftBins))
, fWindow(fFft.getSize() + 1, dsp::WindowingFunction<float>::hann, false)
, fNumMelBins(melBins)
// ^^^^See scipy.filters.get_window and scipy.filter.general_cosine for the
// fftbins parameter which causes the extending of the window by 1, followed
// by a truncation.
{
   fFormatMgr.registerBasicFormats();
}


void ASpectroMaker::AmplitudesToDbs(ASpectrogram& magnitudes)
{
   if (0UL == magnitudes.Size())
   {
      return;
   }
   
   // From libRosa.core.spectrum.power_to_db:859

   // Power to dB while limiting to a minimum magnitude.
   // Using double precision here seems to be necessary to match output of librosa
   TransformChannels(magnitudes, [](float mag)
    {
       // Square values to use power-to-dB calculations.
       constexpr double kMin = 1e-5 * 1e-5;
       constexpr double kReference = 1.0f * 1.0f;
       return 10.0 * std::log10(std::max(kMin, (double)mag * mag))
            - 10.0 * std::log10(std::max(kMin, kReference));
    });

   // Limit to maximum dB
   auto maxDb = magnitudes.MinMax().second - kMaxDb;
   TransformChannels(magnitudes, [maxDb](float dB)
    {
       return std::max(dB, maxDb);
    });
}


void ASpectroMaker::MakeFromFile(ASpectrogram& spectrogram, const File& file)
{
   // How Scott does this (datautils.py:139):
   // melgram = librosa.amplitude_to_db(librosa.feature.melspectrogram(mono_sig,
   //  sr=sr, n_mels=n_mels))[np.newaxis,:,:,np.newaxis]
   // The function Scott's code uses:
   // librosa.feature.melspectrogram(y=None, sr=22050, S=None, n_fft=2048,
   //                                hop_length=512, power=2.0, **kwargs)[source]
   // n_mels is passed through to librosa.features.mel
   // The librosa STFT window defaults to centered Hann
   
   // Scott reads the whole file, produces the full spectrogram, and crops to
   // the graph input size. We are more conservative, but we have to extend
   // a bit past 3 seconds to get the 259 columns given our fft size.
   auto signal = this->ReadAudioSignal(file, 3.1);
   auto hzFft = this->ShortTimeFourier(ToMono(signal).buffer, kHopLength);
   TransformChannels(hzFft, [](auto x) { return x * x; });

   spectrogram.AddChannel(ASpectroMaker::HzToMels(hzFft, 0, signal.sampleRate, fNumMelBins));
   spectrogram.Reshape(fNumMelBins, hzFft.Shape().second);
   ASpectroMaker::AmplitudesToDbs(spectrogram);
}


Signal ASpectroMaker::ReadAudioSignal(const File& file, float lengthSeconds)
{
   std::unique_ptr<juce::AudioFormatReader> reader(fFormatMgr.createReaderFor(file));
   if (!reader)
   {
      throw std::runtime_error("No audio format reader for file type.");
   }

   constexpr double kDesiredSampleRate = 44100.0;
   AudioSampleBuffer buffer(reader->numChannels, kDesiredSampleRate * lengthSeconds);

   if (reader->sampleRate != kDesiredSampleRate)
   {
      const auto origSampleRate = reader->sampleRate;
      const auto numChannels = reader->numChannels;

      auto readerAudioSource = std::make_unique<AudioFormatReaderSource>(reader.release(), true);
      readerAudioSource->setNextReadPosition(0L);

      auto resampler = std::make_unique<ResamplingAudioSource>(readerAudioSource.release(),
                                                               true, numChannels);
      resampler->setResamplingRatio(origSampleRate / kDesiredSampleRate);
      resampler->prepareToPlay(buffer.getNumSamples(), kDesiredSampleRate);

      AudioSourceChannelInfo channelInfo(&buffer, 0, buffer.getNumSamples());
      resampler->getNextAudioBlock(channelInfo);
   }
   else
   {
      reader->read(&buffer, 0, buffer.getNumSamples(), 0L, true, true);
   }
   
   return { buffer, kDesiredSampleRate };
}


ASpectrogram ASpectroMaker::ShortTimeFourier(const AudioSampleBuffer& signal, size_t hop)
{
   assert(signal.getNumChannels() == 1);
   
   const float* data = signal.getReadPointer(0);
   const size_t dataCount = signal.getNumSamples();

   ptrdiff_t fftSize = fFft.getSize();
   ptrdiff_t numHops = 1L + static_cast<long>((dataCount - fftSize) / hop);
   
   size_t numRows = 1UL + (fftSize / 2UL);
   Spectrum spectrum(numRows * numHops, 0.0f);

   // Double bin size to hold complex values though we only get back the real component
   std::vector<float> fftBuffer(fftSize * 2UL);
   
   // Iterator across one row, filling columns of data.
   float* destColumn = spectrum.data();
   const float* destEnd = destColumn + numHops;
   while (destEnd != destColumn)
   {
      if (fftSize > dataCount)
      {
         std::cout << "ASpectroMaker::ShortTimeFourier: Skipping final samples "
          "because there aren't enough for FFT\n";
         break;
      }
      
      std::memcpy(fftBuffer.data(), data, fftSize * sizeof(float));
      
      fWindow.multiplyWithWindowingTable(fftBuffer.data(), fftSize);
      fFft.performFrequencyOnlyForwardTransform(fftBuffer.data());
      
      // Discard the FFT bins beyond numRows (because they are mirrored values).
      auto fftIt = fftBuffer.begin();
      auto fftEnd  = fftIt + numRows;
      for (size_t rowOff = 0UL; fftEnd != fftIt; rowOff += numHops)
      {
         *(destColumn + rowOff) = *fftIt++;
      }
      data += hop;
      destColumn += 1;
   }

   return ASpectrogram({ spectrum }, numRows, (size_t)numHops, "FFT");
}


Spectrum ASpectroMaker::HzToMels(const ASpectrogram& powerData, size_t channel,
                                 float sampleRate, int numMelBins)
{
   int numHzColumns;
   std::tie(std::ignore, numHzColumns) = powerData.Shape();
   
   Spectrum mels(numMelBins * numHzColumns, 0.0f);
   const float* powerIt = powerData.Channel(channel).data();

   AMelFilter melFilter(numMelBins, kNumFftBins, sampleRate);
   melFilter.HzToMels(powerIt, numHzColumns, mels.data());

   return mels;
}



namespace
{
   Signal ToMono(const Signal& signal)
   {
      auto numChannels = signal.buffer.getNumChannels();
      if (numChannels == 1)
      {
         return signal;
      }

      auto numSamples = signal.buffer.getNumSamples();
      auto gain = 1.0 / numChannels;

      AudioSampleBuffer mono(1, numSamples);
      mono.clear();
      for (int c = 0; c < numChannels; ++c)
      {
         mono.addFrom(0, 0, signal.buffer, c, 0, numSamples, gain);
      }
      return { mono, signal.sampleRate };
   }
}
