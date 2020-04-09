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

#include <deque>
#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"

#include "Tag.h"

/**
 * Retrieve file information from persistent storage based on given search criteria.
 */
class AFileSource
{
public:
   
   struct Info
   {
      /** Set file and thumbnail with small thumb. */
      explicit Info(const std::string& filename, int64_t key = -1);
      File file;
      Image thumbnail;
      Tags tags;
      int64_t key;

      // Convenience accessors to File data
      int64_t Size() const;
      String Name() const; //!< Just the file name
      String Path() const; //!< Full path
   };

   size_t LoadFiles(int64_t sortFileId, const Tags& tags);

   Info operator[](size_t index) const;
   
private:
   
   // For quick and easy sorting?
   std::deque<Info> fFiles;
};
