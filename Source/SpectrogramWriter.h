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

#include <string>


#include "Colorizers.h"

class ASpectrogram;

using ImageSizes = std::vector<std::pair<int, int>>;

/**
 * Save spectrograms as images at given sizes.
 *
 * \param sizes The image will be saved at each of the sizes given. The name is
 *              the spectrogram basename with '-<width>x<height>' appended
 *              before '.png'.
 *
 * \return The filenames of the saved PNG files in the same order as the sizes were given.
 */
StringArray SaveAsPng(const ASpectrogram& spectrogram, const File& rootPath,
               const ImageSizes& sizes = {});

/** Save as compressed numpy. */
void SaveAsNpz(const ASpectrogram& spectra, const File& rootPath);
