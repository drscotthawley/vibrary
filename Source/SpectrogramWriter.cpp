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

#include "cnpy.h" // for numpy file support

#include "Spectrogram.h"
#include "SpectrogramWriter.h"

namespace
{
   bool WritePng(const Image& image, const File& file)
   {
      juce::FileOutputStream out(file);
      return juce::PNGImageFormat().writeImageToStream(image, out);
   }
}


void SaveAsNpz(const ASpectrogram& spectra, const File& rootPath)
{
   jassert(spectra.ChannelCount() == 1 &&
           "Only single channel spectrograms can be saved as numpy array");

   const auto file = rootPath.getChildFile(spectra.Basename() + ".npz");

   size_t rows = 0, columns = 0;
   std::tie(rows, columns) = spectra.Shape();

   cnpy::npz_save(file.getFullPathName().toStdString(), "melgram",
                  spectra.Channel(0).data(),
                  { 1UL, rows, columns, 1UL }, "w");
}


StringArray SaveAsPng(const ASpectrogram& spectra, const File& rootPath,
               const ImageSizes& sizes/*={}*/)
{
   juce::Image image { Contrasty(spectra) };

   StringArray filenames;
   for (const auto& size : sizes)
   {
      auto scaled { image.rescaled(size.first, size.second) };

      auto scaledFile = rootPath.getChildFile(spectra.Basename()
                                              + "-" + std::to_string(size.first)
                                              + "x" + std::to_string(size.second)
                                              + ".png");

      WritePng(scaled, scaledFile);

      filenames.add(scaledFile.getFullPathName());
   }
   return filenames;
}

