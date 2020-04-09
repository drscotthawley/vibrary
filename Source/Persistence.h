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

#include <array>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// $$$ Can we find a way to not expose this type?
#include "DatabaseException.h"

using Key = int64_t;
using Keys = std::vector<Key>;
using Filenames = std::unordered_map<Key, std::string>;
/** rowId -> (name, weightsFileName) */
using SortFileInfo = std::unordered_map<Key, std::pair<std::string, std::string>>;
using TagClassId = int;
/** rowId -> (classId, name) */
using TagMap = std::unordered_map<Key, std::pair<TagClassId, std::string>>;
using TagLabels = std::unordered_map<TagClassId, std::string>;
using TagKeys = Keys;


/**
 * This top-level procedural interface is the public interface to persistent storage.
 * All calls that interact with the database may throw ADatabaseException.
 */
namespace Persist
{

   using Exception = ADatabaseException;

   constexpr Key kNoId = -1L;

   constexpr int kUniqueViolation = 2067; // extended error code from SQLite
   
   // Mark: Setup
   
   void Init(const std::string& databaseFilename);
   void CleanUp();
   
   
   // MARK: Adding data
   
   /**
    * Add a file to the database
    * \param filename full filename of the file
    * \returns The ID of the file
    */
   Key AddAudioFile(const std::string& filename);

   /**
    * Tag the audio file with the tag specified.
    */
   Key AddAudioFileTag(Key fileId, Key sortFileId, int classId);
   Key AddAudioFileTag(Key fileId, const std::string& tagName);

   using ThumbNames = std::array<std::string, 2>;
   enum ThumbNameIndex { kThumbLargeIdx, kThumbSmallIdx };
   Key AddAudioThumbnails(Key audioFileId, const ThumbNames& imageFilenames);
   std::string ThumbnailForAudio(const std::string& filename, ThumbNameIndex size);
   std::vector<std::string> ThumbnailsForAudioFiles(const Keys& audioIds);

  /**
   * Remove the audio file, its associations with tags and its associated
   * thumbnail paths.
   */
   void RemoveAudioFilesWithKeys(const Keys& keys);


   /** \param tags Maps classId to name. \return rowID of inserted sort file. */
   Key AddSortFile(const std::string& name, const std::string& filename, const TagLabels& tags);
   
   
   // MARK: Retrieve Data
   
   // TODO: Get the thumbnail along with one or all of these
   Filenames GetAllAudioFiles();
   Filenames GetAudioFilesForTags(const TagKeys& tags);
   Filenames GetAudioFilesForSortFile(Key sortFileId);
   Filenames GetAudioFiles(size_t page, size_t pageSize, const TagKeys& tags); // TODO
   
   
   SortFileInfo GetAllSortFiles();
   std::string GetSortFileName(Key sortFileKey);
   TagMap GetTagsForSortFile(const std::string& name);
   TagMap GetTagsForSortFile(Key sortFileKey);
   TagLabels GetTagsForAudioFile(Key sortFileKey, Key audioFileKey);
   
   
   /** Read a file with strings delimited by newlines into a vector. */
   std::vector<std::string> ReadStrings(const std::string& filename,
                                        bool ignoreBlankLines=true);
}
