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

#include "DataSetBuilder.h"
#include "Paths.h"
#include "RemoteTrainer.h" // for kArchiveName
#include "Spectrogram.h"
#include "SpectrogramWriter.h"


ADataSetBuilder::ADataSetBuilder(const File& rootDirectory) noexcept
: fRootDir(rootDirectory)
{
   jassert(rootDirectory.isDirectory());
}


ASpectrogram ADataSetBuilder::AddFile(const File& file, const String& tag) const noexcept(false)
{
   auto spectra { ASpectrogram(file.getFullPathName().toStdString()) };
   SaveAsNpz(spectra, Paths::MakeDir(fRootDir, tag));
   return spectra;
}


File ADataSetBuilder::MakeDataSetArchive(const String& archiveName, float trainingPercent) const
{
   return ADataSet(fRootDir, trainingPercent).CreateArchive(archiveName.toStdString());
}
