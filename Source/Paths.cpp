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

#include "Paths.h"

namespace Paths
{
   File ApplicationPath()
   {
      return File::getSpecialLocation(File::invokedExecutableFile);
   }
   
   String ApplicationName()
   {
      return (String)ApplicationPath().getFileNameWithoutExtension();
   }
   
   File DataDirectory()
   {
      return File::getSpecialLocation(File::userDocumentsDirectory)
      .getChildFile(ApplicationName());
   }
   
   File DatabaseFile()
   {
      return DataDirectory().getChildFile(ApplicationName() + ".sqlite");
   }

   File HomeDirectory()
   {
      return File::getSpecialLocation(File::SpecialLocationType::userHomeDirectory);
   }

   File HomeDirectoryFile(const String& relativePath)
   {
      return HomeDirectory().getChildFile(relativePath);
   }

   File MakeDir(const File& root, const String& name)
   {
      File dir = root.getChildFile(name);
      Result r = Result::ok();
      if (!dir.exists())
      {
         auto r = dir.createDirectory();
         // Turns out createDirectory reports fail if the dir already exists.
         if (!r.wasOk() && !dir.exists())
         {
            throw std::runtime_error("Failed to create directory: "
                                     + r.getErrorMessage().toStdString());
         }
      }
      return dir;
   }

   File MakeUniqueDir(const File& root, const String& name)
   {
      return MakeDir(root.getNonexistentChildFile(name, ""), "");
   }

   File ResourcePath()
   {
      return ApplicationPath().getChildFile("../../Resources/");
   }

   File SpectrogramDirectory()
   {
      return DataDirectory().getChildFile("Spectrograms");
   }

   File TempDirectory()
   {
      return File::getSpecialLocation(File::tempDirectory);
   }

   File WeightsDirectory()
   {
      return DataDirectory().getChildFile("Weights");
   }
}
