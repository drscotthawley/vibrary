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
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"


/**
 * Takes audio files within a directory, partitions them into training and test
 * sets creating spectrograms for them. These are archived into an archive, then
 * into a tar file.
 *
 * When the data set is deleted, any archive created is also deleted from the
 * file system.
 */
class ADataSet
{
public:
   
   /**
    * \param partitionPercent The percentage of files to put in training set [0,1]
    */
   ADataSet(const File& parentDirectory, float trainingPercent) noexcept(false);

   /**
    * Creates the spectrograms if not there already, saving them as npz files,
    * and partitioning them into a training and testing set. Then it archives
    * the resulting directory tree.
    * \param name The base name of the archive file created.
    * \returns The archive file created from the directory containing the npz files.
    */
   [[nodiscard]] File CreateArchive(const std::string& name) const;

   /** \returns Where the data set creates its files. */
   [[nodiscard]] static File WorkingDirectory();

   /** \returns The name the final archived data set will have. */
   [[nodiscard]] static String MakeArchiveName(const String& baseName);

private:

   void PartitionData(const File& dir, const File& destDir) const;
   void PartitionDir(const File& dir, const File& dest1Dir, const File& dest2Dir) const;
   void PartitionFiles(std::vector<File>& files,
                       const File& dest1Dir, const File& dest2Dir) const;
   
   void AddFile(const File& file, const File& destDir) const noexcept(false);

   const File fParentDir;
   const float fPartitionPercent;
};
