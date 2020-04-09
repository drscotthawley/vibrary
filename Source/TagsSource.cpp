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

#include "Persistence.h"
#include "SortFile.h"
#include "TagsSource.h"

namespace
{
   /**
    * Helper to convert stored info to ATag objects
    * (defined here to avoid exposing Persistence.h in the header)
    */
   void operator<<(Tags& storage, const TagMap& tags);

   void LowercaseCopy(std::string& dest, const std::string& src)
   {
      std::transform(src.begin(), src.end(), dest.begin(), ::tolower);
   }
}


ATagsSource::ATagsSource(const ASortFile& sortFile)
: fSortFileName(sortFile.Name())
{
   fTagInfo << Persist::GetTagsForSortFile(sortFile.Key());
}


const ATag& ATagsSource::operator[](size_t index) const
{
   return fTagInfo.at(index);
}

const Tags& ATagsSource::All() const
{
   return fTagInfo;
}

void ATagsSource::Complete(const std::string& partialTagName, CompletionFunc completed) const
{
   auto lowerName = std::string{partialTagName};
   LowercaseCopy(lowerName, partialTagName);

   for (const auto& tag : fTagInfo)
   {
      auto tagName = std::string{tag.Name()};
      LowercaseCopy(tagName, tagName);

      if (0 == tagName.compare(0, lowerName.size(), lowerName))
      {
         if (!completed(tag))
         {
            break;
         }
      }
   }
}


std::vector<std::string> ATagsSource::Complete(const std::string& partialTagName) const
{
   auto completions = std::vector<std::string>();
   completions.reserve(fTagInfo.size()); // max number of completions
   this->Complete(partialTagName, [&completions](const ATag& tag)
    {
       completions.push_back(tag.Name());
       return true;
     });
   return completions;
}


Tags ATagsSource::Matches(const std::string& name) const
{
   auto matchedTags = Tags();

   auto lowerName = std::string{name};
   std::transform(name.begin(), name.end(), lowerName.begin(), ::tolower);

   std::copy_if(fTagInfo.begin(), fTagInfo.end(), std::back_inserter(matchedTags),
    [lowerName](const ATag& tag)
    {
       auto tagName = std::string{tag.Name()};
       std::transform(tagName.begin(), tagName.end(), tagName.begin(), ::tolower);
       return lowerName == tagName;
    });
   
   return matchedTags;
}


Tags ATagsSource::Matches(int classId) const
{
   auto matchedTags = Tags();
   std::copy_if(fTagInfo.begin(), fTagInfo.end(), std::back_inserter(matchedTags),
    [classId](const ATag& tag)
    {
       return tag.ClassId() == classId;
    });
   
   return matchedTags;
}


std::string ATagsSource::SortFileName() const
{
   return fSortFileName;
}


namespace
{
   void operator<<(Tags& storage, const TagMap& tags)
   {
      storage.reserve(tags.size());
      for (const auto& idTagInfoPair : tags)
      {
         storage.emplace_back(idTagInfoPair.second.second,
                              idTagInfoPair.second.first,
                              idTagInfoPair.first);
      }
   }
}
