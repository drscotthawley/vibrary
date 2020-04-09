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

#include <array>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include "Paths.h"
#include "Persistence.h"
#include "Spectrogram.h"
#include "SpectrogramWriter.h"
#include "StringUtils.h"
#include "TaggingManager.h"
#include "Tagger.h"
#include "Utils.h"


namespace
{
   File SpectroDirForKey(Key fileKey);
}


ATaggingManager::ATaggingManager()
{}


ATaggingManager::ATaggingManager(const ASortFile& sortFile)
: fSortFile(sortFile)
{}


ATaggingManager::ATaggingManager(const ASortFile&& sortFile)
: fSortFile(std::move(sortFile))
{}


int ATaggingManager::TagAndAddFiles(const Array<File>& files,
                                    ProgressFunc progress) noexcept
{
   Array<File> flatFiles { Flatten(files, 1) };

   if (ASortFile::kNoKey == fSortFile.Key())
   {
      fLastError = TRANS("Select a sortfile before adding files.");

      // EARLY EXIT
      return 0;
   }

   auto tagger { ATagger(fSortFile.WeightsFilename()) };

   fLastError.clear();

   const auto numFiles = flatFiles.size();
   int completed = 0;

   int added = 0;
   for (const auto& file : flatFiles)
   {
      const auto filename { file.getFullPathName() };
      try
      {
         auto spectro { ASpectrogram(filename.toStdString()) };

         const auto classId = tagger.TagClassIdFor(spectro);
         const auto fileKey = Persist::AddAudioFile(filename.toStdString());

         Persist::AddAudioFileTag(fileKey, fSortFile.Key(), classId);
         added += 1;

         auto spectroDir { SpectroDirForKey(fileKey) };
         auto pngNames { SaveAsPng(spectro, spectroDir, {{ 65, 54 }, { 27, 17 }}) };

         std::array<std::string, 2> names;
         names[Persist::kThumbLargeIdx] = pngNames[0].toStdString();
         names[Persist::kThumbSmallIdx] = pngNames[1].toStdString();

         Persist::AddAudioThumbnails(fileKey, names);

         // $$$ Profile if rereading the file is the slow part about taggnig.
         progress(++completed, numFiles, File(names[Persist::kThumbLargeIdx]));
      }
      catch (const Persist::Exception& ex)
      {
         if (Persist::kUniqueViolation == ex.Result())
         {
            fLastError += TRANS("File ") + filename + TRANS(" has already been tagged for this sortfile.");
         }
         else
         {
            fLastError += ex.what();
         }
         fLastError += "\n";
      }
      catch (const std::exception& ex)
      {
         fLastError += ex.what() + String("\n");
      }
      catch (...)
      {
         fLastError += TRANS("Unknown exception occurred while tagging ") + filename + "\n";
      }
   }

   if (!fLastError.isEmpty())
   {
      std::cerr << fLastError;
   }

   return added;
}


void ATaggingManager::RemoveFilesIn(const std::vector<Key>& keys)
{
   for (auto key : keys)
   {
      auto thumbDir { SpectroDirForKey(key) };
      if (!thumbDir.deleteRecursively())
      {
         std::cerr << TRANS("Failed to delete to delete thumbnail dir ").toStdString()
                   << thumbDir.getFullPathName() << "\n";
      }
   }

   Persist::RemoveAudioFilesWithKeys(keys);
}


bool ATaggingManager::CanTagFile(const File& file) noexcept
{
   return ATagger::CanTagFile(file);
}


const String& ATaggingManager::LastErrorMessage() const noexcept
{
   return fLastError;
}


void ATaggingManager::SetSortFile(const ASortFile& sortFile) noexcept
{
   fSortFile = sortFile;
}


void ATaggingManager::SetSortFile(const ASortFile&& sortFile) noexcept
{
   fSortFile = std::move(sortFile);
}

namespace
{
   File SpectroDirForKey(Key fileKey)
   {
      return Paths::MakeDir(Paths::SpectrogramDirectory(), String(fileKey));
   }
}
