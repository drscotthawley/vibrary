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
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <stdexcept>

#include "../JuceLibraryCode/JuceHeader.h"

#include "Database.h"
#include "DatabaseCommands.h"
#include "DatabaseException.h"
#include "Paths.h"
#include "PaxArchive.h"
#include "Persistence.h"
#include "Settings.h"


namespace
{
   constexpr auto kAudioFileTableName = "AudioFiles";
   constexpr auto kAudioTagTableName = "AudioTags";
   constexpr auto kSortFileTableName = "SortFiles";
   constexpr auto kTagTableName = "Tags";
   constexpr auto kThumbnailsTableName = "Thumbnails";
   
   constexpr auto kAudioFileIdKey = "audioFileId";
   constexpr auto kClassIdKey = "classId";
   constexpr auto kPrimaryKey = "rowid";
   constexpr auto kNameKey = "name";
   constexpr auto kSortFileKey = "sortFileId";
   constexpr auto kTagIdKey = "tagId";
   constexpr auto kThumbnailKey = "thumbnailId";
   constexpr auto kThumbnailSmallKey = "small";
   constexpr auto kThumbnailLargeKey = "large";
   constexpr auto kWeightsFileKey = "weightsFilename";
   
   constexpr auto kIntType = "INTEGER";
   
   std::unique_ptr<ADatabase> sDatabase;
   
   void InitializeTables() noexcept(false);
   void InsertDefaultSortFiles();
   
   /**
    * Throw a std::runtime_error if result indicates an error occurred. Exception's
    * message is only for the last command executed.
    */
   constexpr void CheckDbResult(int result) noexcept(false);
}

/**
 * Top-level functions to perform the storage and retrieval of the app's information.
 */
namespace Persist
{

void CleanUp()
{
   sDatabase.reset(nullptr);
}


Key AddAudioFile(const std::string& filename) noexcept(false)
{
   ATransaction guard(sDatabase.get());

   // Until we have sort files and tag information in the DB, disable constraint
   // to allow null value for tag ID.
   CheckDbResult(sDatabase->Execute(APragmaCommand("foreign_keys", "OFF")));

   // $$$ Should I query for the file first, then try to add? Or check for it
   // in my current file source's list before control gets here?

   Key primaryKey = 0;
   AInsertCommand addFile(kAudioFileTableName, { AValue(filename) }, { kNameKey });
   auto result = sDatabase->Execute(addFile);
   if (ADatabase::ConstraintUnique(result))
   {
      auto selectAudioFileKey = ASelectFromTableCommand(kAudioFileTableName, { kPrimaryKey })
       .WhereEqual(kNameKey, AValue(filename));
      
      result = sDatabase->Execute(selectAudioFileKey);
      if (0 == result)
      {
         primaryKey = std::stol(selectAudioFileKey.Result(0).at(kPrimaryKey));
      }
   }
   else if (0 == result)
   {
      primaryKey = sDatabase->LastInsertedRowId();
   }
   
   CheckDbResult(result);

   CheckDbResult(sDatabase->Execute(APragmaCommand("foreign_keys", "ON")));

   return primaryKey;
}

   
Key AddAudioFileTag(Key fileId, const std::string& tagName)
{
   ATransaction guard(sDatabase.get());
   
   auto result = sDatabase->Execute(
    AInsertCommand(kAudioTagTableName,
    {
       AValue(fileId),
       ASubqueryValue(ASelectFromTableCommand(kTagTableName, { kPrimaryKey })
       .WhereEqual(kNameKey, AValue(tagName)))
    })
   );

   return (0 == result ? sDatabase->LastInsertedRowId() : kNoId);
}
   

Key AddAudioFileTag(Key fileId, Key sortFileKey, int classId)
{
   ATransaction guard(sDatabase.get());
   
   auto selectTag = ASelectFromTableCommand(kTagTableName, { kPrimaryKey })
    .WhereEqual(kSortFileKey, AValue(sortFileKey))
    .AndEqual(kClassIdKey, AValue(classId));
   
   const auto result = sDatabase->Execute(
    AInsertCommand(kAudioTagTableName, { AValue(fileId), ASubqueryValue(selectTag) },
                                       { kAudioFileIdKey, kTagIdKey }));
   CheckDbResult(result);
   
   return (0 == result ? sDatabase->LastInsertedRowId() : kNoId);
}


Key AddAudioThumbnails(Key audioFileId, const ThumbNames& imageFilenames)
{
   ATransaction guard(sDatabase.get());
   
   auto result = sDatabase->Execute(AInsertCommand(kThumbnailsTableName,
    {
     AValue(imageFilenames[kThumbLargeIdx]),
     AValue(imageFilenames[kThumbSmallIdx])
    },
    { kThumbnailLargeKey, kThumbnailSmallKey }));
   CheckDbResult(result);

   auto thumbnailId = sDatabase->LastInsertedRowId();

   result = sDatabase->Execute(AUpdateCommand(kAudioFileTableName,
                                              AValue(audioFileId),
                                              kThumbnailKey,
                                              AValue(thumbnailId)));
   CheckDbResult(result);

   return thumbnailId;
}


Key AddSortFile(const std::string& name, const std::string& filename, const TagLabels& tags)
{
   auto sortFileId = kNoId;
   
   ATransaction guard(sDatabase.get());
   
   AInsertCommand addFile(kSortFileTableName, { AValue(name), AValue(filename) });
   auto result = sDatabase->Execute(addFile);
   if (0 == result)
   {
      sortFileId = sDatabase->LastInsertedRowId();
      auto sortFileIdVal = AValue(sortFileId);
      for (auto tagPair : tags)
      {
         auto classIdVal = AValue(tagPair.first);
         auto nameVal = AValue(tagPair.second);
         AInsertCommand addTag(kTagTableName, { nameVal, sortFileIdVal, classIdVal },
                               { kNameKey, kSortFileKey, kClassIdKey });
         sDatabase->Execute(addTag);
      }
   }
   else
   {
      // TODO: Handle errors. This is also reached when UNIQUE constraint isn't met.
   }
   return sortFileId;
}



void RemoveAudioFilesWithKeys(const Keys& keys)
{
   ATransaction guard(sDatabase.get());

   CheckDbResult(sDatabase->Execute(
    ADeleteFromTable(kAudioFileTableName, AIn(kPrimaryKey, keys))
   ));
}


Filenames GetAllAudioFiles()
{
   auto filenames = Filenames();
   
   // TODO: Left join and retrieve the thumbnail at once?
   // TODO: Paging
   auto getAllAudioFiles =
    ASelectFromTableCommand(kAudioFileTableName,
     {
        kPrimaryKey,
        kNameKey
     })
    .OrderBy({ kNameKey });

   if (0 == sDatabase->Execute(getAllAudioFiles))
   {
      auto results = getAllAudioFiles.Results();
      filenames.reserve(results.size());
      for (auto result : results)
      {
         filenames[std::stol(result[kPrimaryKey])] = result[kNameKey];
      }
   }
   return filenames;
}
   
   
Filenames GetAudioFilesForSortFile(Key sortFileId)
{
   auto filenames = Filenames();
   
   // TODO: join and retrieve the thumbnail at once?
   auto getFiles =
    // Get filenames for the subset of audio file IDs returned from subquery.
    ASelectFromTableCommand(kAudioFileTableName,
    {
       ATableKey(kAudioFileTableName, kPrimaryKey),
       ATableKey(kAudioFileTableName, kNameKey)
    })
    .WhereIn(kPrimaryKey,
    {
       // Get audio file IDs associated with any of the sort file tags.
       ASelectFromTableCommand(kAudioTagTableName, { ATableKey(kAudioTagTableName, kAudioFileIdKey) })
       .Distinct()
       .WhereIn(ATableKey(kAudioTagTableName, kTagIdKey),
       {
          // Get tag IDs for the given sort file.
          ASelectFromTableCommand(kTagTableName, { ATableKey(kTagTableName, kPrimaryKey) })
          .WhereEqual(ATableKey(kTagTableName, kSortFileKey), AValue(sortFileId))
       })
    })
   .OrderBy({ ATableKey(kAudioFileTableName, kNameKey) });

   if (0 == sDatabase->Execute(getFiles))
   {
      auto results = getFiles.Results();
      filenames.reserve(results.size());
      for (auto result : results)
      {
         filenames[std::stol(result[kPrimaryKey])] = result[kNameKey];
      }
   }
   return filenames;
}
   

Filenames GetAudioFilesForTags(const TagKeys& tags)
{
   auto filenames = Filenames();

   auto getFiles =
    ASelectFromTableCommand(kAudioFileTableName,
    {
       ATableKey(kAudioFileTableName, kPrimaryKey),
       ATableKey(kAudioFileTableName, kNameKey)
    })
    .Join(kAudioTagTableName,
          AEquals(ATableKey(kAudioFileTableName, kPrimaryKey),
                  ATableKey(kAudioTagTableName, kAudioFileIdKey)));

   auto tagIdStrs = std::vector<std::string>();
   if (!tags.empty())
   {
      tagIdStrs.reserve(tags.size());
      for (const auto& tagId : tags)
      {
         tagIdStrs.push_back(std::to_string(tagId));
      }
      getFiles.WhereIn(kTagIdKey, tagIdStrs);
   }

   if (0 == sDatabase->Execute(getFiles))
   {
      auto results = getFiles.Results();
      filenames.reserve(results.size());
      for (auto result : results)
      {
         filenames[std::stol(result.at(kPrimaryKey))] = result.at(kNameKey);
      }
   }
 
   return filenames;
}

   
SortFileInfo GetAllSortFiles()
{
   auto sortFiles = SortFileInfo();
   
   auto getSortFiles = ASelectFromTableCommand(kSortFileTableName,
    { kPrimaryKey, kNameKey, kWeightsFileKey })
   .OrderBy({ kNameKey });
   
   if (0 == sDatabase->Execute(getSortFiles))
   {
      auto results = getSortFiles.Results();
      sortFiles.reserve(results.size());
      for (auto result : results)
      {
         sortFiles[std::stol(result.at(kPrimaryKey))] =
          { result[kNameKey], result[kWeightsFileKey] };
      }
   }
   return sortFiles;
}


std::string GetSortFileName(Key sortFileKey)
{
   auto getSortFileName = ASelectFromTableCommand(kSortFileTableName, { kNameKey })
    .WhereEqual(kPrimaryKey, AValue(sortFileKey));
   
   std::string name;
   if (0 == sDatabase->Execute(getSortFileName))
   {
      name = getSortFileName.Result(0).at(kNameKey);
   }
   return name;
}


TagLabels GetTagsForAudioFile(Key sortFileId, Key audioFileId)
{
   auto tags = TagLabels();

   // SELECT Tags.name from AudioTags
   //  JOIN Tags ON Tags.rowId=AudioTags.tagId
   //  WHERE AudioTags.audioFileId='188';


   auto filter = AConditional{ AEquals(ATableKey(kAudioTagTableName, kAudioFileIdKey),
                                       AValue(audioFileId)) };

   auto getTagNames =
    ASelectFromTableCommand(kAudioTagTableName,
     {
        ATableKey(kTagTableName, kClassIdKey),
        ATableKey(kTagTableName, kNameKey)
     })
    .Join(kTagTableName,
          AEquals(ATableKey(kTagTableName, kPrimaryKey),
                  ATableKey(kAudioTagTableName, kTagIdKey)))
   .Where(sortFileId <= 0 ? filter :
          filter.And(AEquals(ATableKey(kTagTableName, kSortFileKey),
                             AValue(sortFileId))));

   if (0 == sDatabase->Execute(getTagNames))
   {
      auto results = getTagNames.Results();
      tags.reserve(results.size());
      for (auto result : results)
      {
         tags[std::stoi(result.at(kClassIdKey))] = result[kNameKey];
      }
   }

   return tags;
}

TagMap GetTagsForSortFile(const std::string& name)
{
   //$$$ We can do this in a join instead of two requests if optimizing is ever
   // needed.
   
   auto getSortFileId = ASelectFromTableCommand(kSortFileTableName, { kPrimaryKey })
    .WhereEqual(kNameKey, AValue(name));
   
   if (0 == sDatabase->Execute(getSortFileId))
   {
      const Key sortFileKey = std::stol(getSortFileId.Result(0).at(kPrimaryKey));
      return GetTagsForSortFile(sortFileKey);
   }
   else
   {
      return TagMap();
   }
}


TagMap GetTagsForSortFile(Key sortFileKey)
{
   using namespace std;
   
   auto outTags = TagMap();
   
   auto getTags = ASelectFromTableCommand(kTagTableName,
    { kPrimaryKey, kClassIdKey, kNameKey})
    .WhereEqual(kSortFileKey, AValue(sortFileKey));
   
   if (0 == sDatabase->Execute(getTags))
   {
      auto results = getTags.Results();
      outTags.reserve(results.size());
      for (auto result : results)
      {
         outTags[stol(result.at(kPrimaryKey))] =
          { stoi(result[kClassIdKey]), result[kNameKey] };
      }
   }
   return outTags;
}


void Init(const std::string& databaseFilename)
{
   sDatabase = std::make_unique<ADatabase>(databaseFilename);
   InitializeTables();
   InsertDefaultSortFiles();
}


std::vector<std::string> ReadStrings(const std::string& filename, bool ignoreBlankLines/*=true*/)
{
   using namespace std;
   
   vector<string> strings;
   ifstream input(filename);
   constexpr size_t kMaxPath = 4096;
   array<char, kMaxPath> line;
   while (input.good() && !input.eof())
   {
      input.getline(line.data(), kMaxPath);
      if (input.fail() && !input.eof())
      {
         throw std::runtime_error("Error in ReadStrings(): Insufficient memory "
                                  "to read a line of text.");
      }
      auto beginLine = begin(line);
      auto endStr = find(beginLine, end(line), '\0');
      if (!ignoreBlankLines || 0 != (endStr - beginLine))
      {
         strings.emplace_back(beginLine, endStr);
      }
   }
   return strings;
}


std::string ThumbnailForAudio(const std::string& filename, ThumbNameIndex sizeIdx)
{
   auto thumbKey = kThumbnailLargeKey;
   if (kThumbSmallIdx == sizeIdx)
   {
      thumbKey = kThumbnailSmallKey;
   }

   std::string thumbFilename;


#if 1 // I don't know which is more efficient, perhaps both are the same.

   // JOIN version
   auto s = ASelectFromTableCommand(kThumbnailsTableName,
                                    { ATableKey(kThumbnailsTableName, thumbKey) })
    .Join(kAudioFileTableName,
          AEquals(ATableKey(kThumbnailsTableName, kPrimaryKey),
                  ATableKey(kAudioFileTableName, kThumbnailKey)))
    .WhereEqual(ATableKey(kAudioFileTableName, kNameKey), AValue(filename));

#else

   // SUBQUERY version
   auto s = ASelectFromTableCommand(kThumbnailsTableName, { thumbKey })
   .WhereEqual(kPrimaryKey,
               ASubqueryValue(ASelectFromTableCommand(kAudioFileTableName, { kPrimaryKey })
                              .WhereEqual(kNameKey, AValue(filename))
                              )
               );

#endif

   if (0 == sDatabase->Execute(s))
   {
      thumbFilename = s.Result(0).at(thumbKey);
   }
   return thumbFilename;
}


std::vector<std::string> ThumbnailsForAudioFiles(const Keys& audioIds)
{
   std::vector<std::string> paths;

   auto columns = ASelectFromTableCommand::ColumnNames { kThumbnailLargeKey, kThumbnailSmallKey };

   auto getThumbPaths =
    ASelectFromTableCommand(kThumbnailsTableName, columns)
    .Where(AIn(kAudioFileIdKey, audioIds));

   auto result = sDatabase->Execute(getThumbPaths);
   if (0 == result)
   {
      paths.reserve(getThumbPaths.Results().size() * columns.size());
      for (const auto& row : getThumbPaths.Results())
      {
         for (const auto& key : columns)
         {
            paths.push_back(row.at(key));
         }
      }
   }
   return paths;
}

}

namespace
{
   constexpr void CheckDbResult(int result) noexcept(false)
   {
      if (0 != result)
      {
         throw ADatabaseException(sDatabase->LastErrorMessage(), result);
      }
   }
   
   void InitializeTables() noexcept(false)
   {
      ATransaction transactionGuard(sDatabase.get());
      try
      {
         auto createThumbnailsTable = ACreateTableCommand(kThumbnailsTableName);
         createThumbnailsTable.AddColumns(
          {
           AColumn(kThumbnailLargeKey).Unique(),
           AColumn(kThumbnailSmallKey).Unique()
          });
         CheckDbResult(sDatabase->Execute(createThumbnailsTable));

         auto createSortTable = ACreateTableCommand(kSortFileTableName);
         createSortTable.AddColumns(
          {
           AColumn(kNameKey).Unique(),
           AColumn(kWeightsFileKey).Unique()
          });
         CheckDbResult(sDatabase->Execute(createSortTable));
 
         auto createTagsTable = ACreateTableCommand(kTagTableName);
         createTagsTable.AddColumns(
          {
           kNameKey,
           AColumn(kClassIdKey, kIntType),
           kSortFileKey
          });
         createTagsTable.AddRelation(kSortFileKey, kSortFileTableName, kPrimaryKey);
         CheckDbResult(sDatabase->Execute(createTagsTable));

         auto createAudioTable = ACreateTableCommand(kAudioFileTableName);
         createAudioTable.AddColumns(
          {
           AColumn(kNameKey).Unique(),
           AColumn(kThumbnailKey, kIntType)
          });
         createAudioTable.AddRelation(kThumbnailKey, kThumbnailsTableName, kPrimaryKey);
         CheckDbResult(sDatabase->Execute(createAudioTable));
   
         auto createAudioTagsTable = ACreateTableCommand(kAudioTagTableName);
         createAudioTagsTable.AddColumns(
          {
           AColumn(kAudioFileIdKey, kIntType),
           AColumn(kTagIdKey, kIntType),
           AUniqueConstraint({ kAudioFileIdKey, kTagIdKey })
          });
         createAudioTagsTable.AddRelation(kAudioFileIdKey, kAudioFileTableName, kPrimaryKey);
         createAudioTagsTable.AddRelation(kTagIdKey, kTagTableName, kPrimaryKey);
         CheckDbResult(sDatabase->Execute(createAudioTagsTable));

         CheckDbResult(sDatabase->Execute(
          ACreateTriggerCommand("DeleteAudioData", kAudioFileTableName,
           Trigger::kAfter, Trigger::kDelete,
           Trigger::IsTemp(false), Trigger::ForEachRow(),
           {
            ADeleteFromTable(kAudioTagTableName,
                             AEquals(kAudioFileIdKey, ASchemaOld(kPrimaryKey))),
            ADeleteFromTable(kThumbnailsTableName,
                             AEquals(kPrimaryKey, ASchemaOld(kThumbnailKey)))
           })
         ));

      }
      catch (...)
      {
         transactionGuard.Rollback();
      }
   }
   
   void InsertDefaultSortFiles()
   {
      ATransaction transactionGuard(sDatabase.get());
      
      DirectoryIterator dirIter(Paths::ResourcePath(), false, "*", File::findFiles);
      bool isDirectory;
      bool isHidden;
      while (dirIter.next(&isDirectory, &isHidden, nullptr, nullptr, nullptr, nullptr))
      {
         auto file = dirIter.getFile();
         if (file.hasFileExtension(".zip"))
         {
            auto baseName = file.getFileNameWithoutExtension();

            // If we make this a unique name, we have to make sure child names
            // match for .bp and .txt files. So for now, just
            auto destDir = Paths::WeightsDirectory().getChildFile(baseName);

            const auto replace = String("Weights");
            auto replaceStart = baseName.lastIndexOf(replace);
            if (-1 == replaceStart)
            {
               replaceStart = baseName.length();
               baseName += replace;
            }

            // We need the path to the protobuf and its associated text file
            // with label names. The convertion now is to replace Weights
            // with Labels and add the extension.

            auto pbRootDir = destDir.getChildFile(baseName + ".pb");
            auto labelBaseName = baseName.replaceSection(replaceStart,
                                                         replace.length(),
                                                         "Labels.txt");
            auto labelFilename = destDir.getChildFile(labelBaseName);

            if (!destDir.exists() || !pbRootDir.exists())
            {
               destDir.createDirectory();

               APaxExtractor::ExtractAll(file, destDir);

               auto labels = Persist::ReadStrings(labelFilename.getFullPathName().toStdString());
               if (!labels.empty())
               {
                  auto tagLabels = TagLabels();
                  tagLabels.reserve(labels.size());
                  for (int i = 0; i < labels.size(); ++i)
                  {
                     tagLabels[i] = labels[i];
                  }

                  const auto name = file.getFileNameWithoutExtension().toStdString();
                  const auto location = pbRootDir.getFullPathName().toStdString();
                  Persist::AddSortFile(name, location, tagLabels);
               }
               else
               {
                  // TODO: Report error that labels cannot be empty. Clean up work.
               }
            }
         }
      }
   }
}
