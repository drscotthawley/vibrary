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
#include <string>
#include <vector>

#include "Tag.h"

class ASortFile;

/** Provides tags defined in a given sort file */
class ATagsSource
{
public:
   /** Load tags in no particular order. */
   explicit ATagsSource(const ASortFile& sortFile);
   ATagsSource() = default;
   
   std::string SortFileName() const;
   
   const ATag& operator[](size_t index) const;
   const Tags& All() const;
   
   /** \return List of tag names that start with partialTagName */
   std::vector<std::string> Complete(const std::string& partialTagName) const;
   
   /**
    * \param Func A functor that takes an ATag object.
    * \returns True to continue completing, false to stop completing.
    */
   using CompletionFunc = std::function<bool(const ATag&)>;
   void Complete(const std::string& partialTagName, CompletionFunc completed) const;
   
   Tags Matches(const std::string& name) const;
   Tags Matches(int classId) const;
   
private:

   std::string fSortFileName;
   std::vector<ATag> fTagInfo;
};

