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

#include "FileSource.h"
#include "Persistence.h"

namespace
{
   Image uLoadThumbnail(const std::string& audioFile)
   {
      Image image;
      try
      {
         image = ImageCache::getFromFile(
          File(Persist::ThumbnailForAudio(audioFile, Persist::kThumbSmallIdx)
               .c_str()));
      }
      catch (const std::exception& e)
      {
         std::cerr << "Failed to get thumbnail for " << audioFile
          << "\n\texception: " << e.what() << "\n";
      }
      return image;
   }
}


AFileSource::Info AFileSource::operator[](size_t index) const
{
   return fFiles.at(index);
}

size_t AFileSource::LoadFiles(int64_t sortFileId, const Tags& tags)
{
   // TODO: Paging to reduce memory use?
   
   auto filenames = Filenames();
   auto tagInfo = std::map<std::string, Tags>();

   if (tags.empty() && sortFileId <= 0L)
   {
      filenames = Persist::GetAllAudioFiles();
   }
   else if (tags.empty())
   {
      filenames = Persist::GetAudioFilesForSortFile(sortFileId);
   }
   else
   {
      auto tagIds = TagKeys();

      tagIds.reserve(tags.size());
      for (const auto& tag : tags)
      {
         tagIds.push_back(tag.Key());
      }
      filenames = Persist::GetAudioFilesForTags(tagIds);
   }
   
   fFiles.clear();
   for (const auto& keyNamePair : filenames)
   {
      const auto& fileId = keyNamePair.first;
      const auto& name = keyNamePair.second;
      auto& info = fFiles.emplace_back(name, fileId);

      const auto tagLabels = Persist::GetTagsForAudioFile(sortFileId, fileId);
      std::transform(tagLabels.begin(), tagLabels.end(), std::back_inserter(info.tags),
       [](const auto& classNamePair)
       {
          return ATag(classNamePair.second, classNamePair.first);
       });
   }
   
   return fFiles.size();
}


AFileSource::Info::Info(const std::string& filename, int64_t fileId/*=-1*/)
: file{filename}
, thumbnail{uLoadThumbnail(filename)}
, key(fileId)
{}

String AFileSource::Info::Name() const
{
   return file.getFileName();
}

String AFileSource::Info::Path() const
{
   return file.getFullPathName();
}

int64_t AFileSource::Info::Size() const
{
   return file.getSize();
}
