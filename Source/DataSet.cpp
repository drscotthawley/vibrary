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

#include <random>
#include <algorithm>
#include <iostream>

#include "DataSet.h"
#include "Paths.h"
#include "PaxArchive.h"
#include "Spectrogram.h"
#include "SpectrogramWriter.h"
#include "Tagger.h"

// TODO: Get LOG(...) working in here instead of iostream


ADataSet::ADataSet(const File& parentDirectory, float percent)
: fParentDir(parentDirectory)
, fPartitionPercent(percent)
{
   if (!parentDirectory.isDirectory())
   {
      throw std::runtime_error("ADataSet requires a directory, but a file was provided.");
   }
}


void ADataSet::AddFile(const File& file, const File& destDir) const
{
   try
   {
      if (ATagger::CanTagFile(file))
      {
         SaveAsNpz(ASpectrogram(file.getFullPathName().toStdString()),
                   destDir);
      }
      else if (file.existsAsFile())
      {
         file.moveFileTo(destDir.getChildFile(file.getFileName()));
      }
   }
   catch (const std::runtime_error& ex)
   {
      std::cerr << "ADataSet did not add " << file.getFullPathName() << ": "
                << ex.what() << "\n";
      throw ex;
   }
}


File ADataSet::CreateArchive(const std::string& name) const
{
   File destDir { ADataSet::WorkingDirectory().getChildFile(name) };
   destDir.createDirectory();

   this->PartitionData(fParentDir, destDir);

   File archive = APaxMaker::MakeFrom(destDir);
   
   if (!destDir.deleteRecursively())
   {
      std::cerr << "Failed to remove ADataSet spectrogram directory "
                << destDir.getFullPathName().toStdString() << "\n";
   }

   return archive;
}


String ADataSet::MakeArchiveName(const String& baseName)
{
   return APaxMaker::MakeArchiveName(baseName);
}


void ADataSet::PartitionData(const File& dir, const File& destDir) const
{
   File trainDest = destDir.getChildFile("Train");
   trainDest.createDirectory();
   
   File testDest = destDir.getChildFile("Test");
   testDest.createDirectory();
   
   this->PartitionDir(dir, trainDest, testDest);
}


void ADataSet::PartitionDir(const File& dir, const File& dest1Dir, const File& dest2Dir) const
{
   DirectoryIterator dirIter(dir, false, "*", File::findFilesAndDirectories);
   bool isDirectory;
   bool isHidden;
   std::vector<File> files;
   std::vector<File> dirs;
   while (dirIter.next(&isDirectory, &isHidden, nullptr, nullptr, nullptr, nullptr))
   {
      if (!isHidden)
      {
         (isDirectory ? dirs : files).push_back(dirIter.getFile());
      }
   }
   
   this->PartitionFiles(files, dest1Dir, dest2Dir);
   
   for (const auto& nextDir : dirs)
   {
      File nextDest1Dir(dest1Dir.getChildFile(nextDir.getFileName()));
      nextDest1Dir.createDirectory();
      
      File nextDest2Dir(dest2Dir.getChildFile(nextDir.getFileName()));
      nextDest2Dir.createDirectory();
      
      this->PartitionDir(nextDir, nextDest1Dir, nextDest2Dir);
   }
}

                         
void ADataSet::PartitionFiles(std::vector<File>& files,
                              const File& dest1Dir, const File& dest2Dir) const
{
#if 1
   
   std::random_device rd;
   std::mt19937 g(rd());
   std::shuffle(files.begin(), files.end(), g);
   
   auto fileIt = files.begin();
   auto fileEnd = fileIt + static_cast<size_t>(files.size() * fPartitionPercent);
   
   while (fileIt != fileEnd)
   {
      this->AddFile(*fileIt++, dest1Dir);
   }
   
   fileEnd = files.end();
   while (fileIt != fileEnd)
   {
      this->AddFile(*fileIt++, dest2Dir);
   }
   
#else
   
   const auto part1Size = static_cast<size_t>(files.size() * fPartitionPercent);
   PartitionInto(*this, part1Size, files.begin(), files.end(), dest1Dir, dest2Dir);
   
#endif
}


File ADataSet::WorkingDirectory()
{
   return Paths::TempDirectory();
}


#if 0 // I thought I would need a version that worked on another container type
      
template <class Iter>
void PartitionInto(ADataSet& set, size_t part1Size, const Iter& start, const Iter& end,
                   const File& dest1Dir, const File& dest2Dir)
{
   assert(part1Size < (size_t)(end - start));
   
   std::random_device rd;
   std::mt19937 g(rd());
   std::shuffle(start, end, g);
   
   const auto partitionEnd = start + part1Size;
   
   while (start != partitionEnd)
   {
      set.AddFile(*start++, dest1Dir);
   }
   
   while (start != end)
   {
      set.AddFile(*start++, dest2Dir);
   }
}

#endif
