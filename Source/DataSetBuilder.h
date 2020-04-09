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

#include "../JuceLibraryCode/JuceHeader.h"

#include "DataSet.h"
#include "Spectrogram.h"


/**
 * Allows for 'piece-wise' accumulation of a data set rather than
 * ADataSet's one-shot creation from a prepared directory.
 */
class ADataSetBuilder
{
public:
   /**
    * \param rootDirectory Where to build the data set.
    */
   explicit ADataSetBuilder(const File& rootDirectory) noexcept;
   
   ASpectrogram AddFile(const File& file, const String& tag) const noexcept(false);
   
   /**
    * \returns The rootDirectory populated with the tags and spectrograms as npz files,
    *          archived as a pax file. See ADataSet::CreateArchive().
    */
   File MakeDataSetArchive(const String& archiveName, float trainingPercent) const;


private:

   const File fRootDir;
};

