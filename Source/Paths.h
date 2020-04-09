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


/**
 * Paths and related information used by the application.
 */
namespace Paths
{
   
   /**
    * \returns The path to this executable.
    */
   [[nodiscard]] File ApplicationPath();
   
   /**
    * \returns Just the name of the application, extracted from ApplicationPath().
    */
   [[nodiscard]] String ApplicationName();
   
   /**
    * \returns The directory containing the database file, trained weights,
    *          and any training data like spectrograms.
    */
   [[nodiscard]] File DataDirectory();

   /**
    * \returns The directory containing spectrogram images.
    */
   [[nodiscard]] File SpectrogramDirectory();

   /**
    * \return Directory where model weights are stored.
    */
   [[nodiscard]] File WeightsDirectory();

   /**
    * \returns The path to the user's home directory.
    */
   [[nodiscard]] File HomeDirectory();

   [[nodiscard]] File ResourcePath();
   
   /**
    * \returns A writable directory suitable for temporary files.
    */
   [[nodiscard]] File TempDirectory();
   
   /**
    * \returns The full path to the database file.
    */
   [[nodiscard]] File DatabaseFile();


   /** \return HomeDirectory() + relativePath */
   [[nodiscard]] File HomeDirectoryFile(const String& relativePath);

   /**
    * Utility to create a directory under rootDir
    */
   [[nodiscard]] File MakeDir(const File& rootDir, const String& name) noexcept(false);
   [[nodiscard]] File MakeUniqueDir(const File& rootDir, const String& name) noexcept(false);
}
