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
 * ASortFile is an association of a name/id with the weights and the tags
 * produced by a neural net when initialized with these weights.
 *
 * \note This is only conceptually a file once entered into the database. The
 * information does not live in its own file in the OS filesystem.
 */
class ASortFile
{
public:
   constexpr static int kNoKey = -1;

   ASortFile(const std::string& name = std::string(),
             const std::string& weights = std::string(),
             int key = kNoKey);

   constexpr bool operator==(const ASortFile& other) const noexcept { return fKey == other.fKey; }
   constexpr const std::string& Name() const noexcept { return fName; }
   constexpr const std::string& WeightsFilename() const noexcept { return fWeightsFilename; }
   constexpr int Key() const noexcept { return fKey; }

   // TODO: Labels() Lazily load the labels? See ATagsSource

private:
   std::string fName;
   std::string fWeightsFilename;
   int fKey;
};
