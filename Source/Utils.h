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

#include <functional>
#include <iostream>

#include "../JuceLibraryCode/JuceHeader.h"

/**
 * \Return the list of files including any contents of directories up to a
 *         given depth.
 */
Array<File> Flatten(const Array<File>& files, int toDepth);


/**
 * \Return a new list containing only items from files for which f returns true.
 */
using FileFilterFunc = std::function<bool(const File&)>;
Array<File> Filter(const Array<File>& files, FileFilterFunc f);


/**
 * Custom deleter that also removes a directory from the filesystem.
 */
struct ADirRemover
{
   ADirRemover(String msg) : fMsg(msg.toStdString())
   {}

   void operator()(File* dir) const
   {
      jassert(dir->isDirectory());
      if (!dir->deleteRecursively())
      {
         std::cerr << fMsg << ": " << dir->getFullPathName() << "\n";
      }
      delete dir;
   }
   const std::string fMsg;
};
