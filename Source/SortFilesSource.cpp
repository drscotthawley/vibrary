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

#include "Persistence.h"
#include "SortFilesSource.h"


ASortFilesSource::ASortFilesSource()
{
   auto sortFileInfo = Persist::GetAllSortFiles();
   
   fSortFiles.reserve(sortFileInfo.size());
   
   for (const auto& idInfoPair : sortFileInfo)
   {
      fSortFiles.emplace_back(idInfoPair.second.first,  // name
                              idInfoPair.second.second, // weights
                              idInfoPair.first);        // key
   }
}


const ASortFile& ASortFilesSource::operator[](size_t index) const
{
   return fSortFiles.at(index);
}


size_t ASortFilesSource::Size() const noexcept
{
   return fSortFiles.size();
}
