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

#include "Utils.h"

Array<File> Filter(const Array<File>& files, FileFilterFunc shouldAdd)
{
   Array<File> filtered;
   auto it = files.begin();
   const auto last = files.end();
   while (last != it)
   {
      const auto& file = *it++;
      if (shouldAdd(file))
      {
         filtered.add(file);
      }
   }
   return filtered;
}


Array<File> Flatten(const Array<File>& files, int toDepth)
{
  Array<File> flat;
  auto it = files.begin();
  const auto last = files.end();
  while (last != it)
  {
     if (it->isDirectory())
     {
        DirectoryIterator dirIt{*it, false, "*", File::findFilesAndDirectories};
        while (dirIt.next())
        {
           const auto& fileOrDir = dirIt.getFile();
           if (fileOrDir.isDirectory())
           {
              if (toDepth > 0)
              {
                 flat.addArray(Flatten({ fileOrDir }, toDepth - 1));
              }
           }
           else
           {
              flat.add(fileOrDir);
           }
        }
     }
     else
     {
        flat.add(*it);
     }
     ++it;
  }
  return flat;
}
