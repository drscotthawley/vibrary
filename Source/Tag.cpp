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

#include "Tag.h"


constexpr int ATag::kNoId;


ATag::ATag()
: fClassId(-1)
, fKey(-1)
{}

ATag::ATag(const std::string& name, int classId/*=-1*/, int64_t key/*=-1L*/)
: fName(name)
, fClassId(classId)
, fKey(key)
{}

bool operator<(const ATag& a, const ATag& b)
{
   return a.Name() < b.Name();
}

bool operator==(const ATag& a, const ATag& b)
{
   return a.ClassId() == b.ClassId();
}
