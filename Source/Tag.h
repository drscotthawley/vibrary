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


/**
 * The tag describes the category the net assigned to a file.
 */
class ATag
{
public:
   constexpr static int kNoId = -1;
   
   ATag();
   ATag(const std::string& name, int classId = -1, int64_t key = -1L);
   
   constexpr const std::string& Name() const noexcept
   {
      return fName;
   };

   /** The id of this tag in the sort file defining it, relating it to the net's output. */
   constexpr int ClassId() const noexcept
   {
      return fClassId;
   }
 
   /** The primary key for the tag in the database, unique among all tags. */
   constexpr int64_t Key() const noexcept
   {
      return fKey;
   };
   
private:
   std::string fName;
   int fClassId;
   int64_t fKey;
};

/** Order by name */
bool operator<(const ATag& a, const ATag& b);
/** Equality by name (note classId is not unique among all tags) */
bool operator==(const ATag& a, const ATag& b);


using Tags = std::vector<ATag>;
