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

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

class ASpectroMaker;


class ASpectrogram
{
public:
   using Spectrum = std::vector<float>;   //!< A matrix, bins x (duration / hop_size)
   using Spectra = std::vector<Spectrum>; //!< spectrum per channel
   static constexpr size_t kNumMelBins = 96UL;

   /** \param audioFilename Path to the audio file. */
   ASpectrogram(const std::string& audioFilename);
   
   /** Clean up static objects shared among spectrograms */
   static void CleanUp();
   
   const Spectrum& Channel(size_t index) const;
   Spectrum& Channel(size_t index);
   size_t ChannelCount() const;
   void SetChannelSpectrum(size_t index, const Spectrum&& data);
   
   // Numpy like operations
   std::pair<size_t, size_t> Shape() const { return { fRows, fColumns }; }
   size_t Size() const { return fRows * fColumns; };
   std::pair<float, float> MinMax() const;  //!< \returns <min, max>

   /** The basename of the path this spectrogram was constructed with. */
   constexpr const std::string& Basename() const noexcept { return fBasename; }
   
   /** For testing, force the spectra to the given values. */
   ASpectrogram(const Spectra& spectra, size_t rows, size_t columns, const std::string& basename);
   /** For testing, load the first array in the npz into a Spectrogram */
   static ASpectrogram FromNpz(const std::string& npzFlename);
   
private:
   friend class ASpectroMaker;
   
   /** \throws std::runtime_exception if (rows * columns) != the current size */
   void Reshape(size_t rows, size_t columns);

   void AddChannel(const Spectrum& spectrum);
   void AddChannel(const Spectrum&& spectrum);
   std::string ConstructPath(const std::string& rootPath, const std::string& basename,
                             const std::string& extension) const;
 
   const std::string fBasename; // input
   Spectra fSpectra;
   
   size_t fRows;
   size_t fColumns;
   
   static std::unique_ptr<ASpectroMaker> sSpectroMaker;
};


template <typename Func>
void ForEachChannel(ASpectrogram& spectro, Func op)
{
   for (size_t c = 0; c < spectro.ChannelCount(); ++c)
   {
      auto& channel = spectro.Channel(c);
      std::for_each(std::begin(channel), std::end(channel), op);
   }
}

template <typename Func>
void TransformChannels(ASpectrogram& spectro, Func unaryOp)
{
   for (size_t c = 0; c < spectro.ChannelCount(); ++c)
   {
      auto& channel = spectro.Channel(c);
      std::transform(std::begin(channel), std::end(channel), std::begin(channel), unaryOp);
   }
}
