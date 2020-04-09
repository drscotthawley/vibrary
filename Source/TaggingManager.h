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
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"

#include "SortFile.h"


/**
 * Handles predicting the tag for a file with the given sort file, storing
 * artifacts and updating the database.
 */
class ATaggingManager
{
public:
   explicit ATaggingManager(const ASortFile& sortFile);
   explicit ATaggingManager(const ASortFile&& sortFile);
   ATaggingManager(); //!< If constructed this way, you must call SetSortFile before using AddFiles.

   /**
    * \return The number of files successfully added. Also adds any errors or
    *         warnings to fLastError.
    */
   using ProgressFunc = std::function<void(int completed, int total, const File& smallSpectraImage)>;
   int TagAndAddFiles(const Array<File>& files, ProgressFunc) noexcept;

   void RemoveFilesIn(const std::vector<int64_t>& keys);

   /** Any error messages from the last call to TagAndAddFiles. */
   const String& LastErrorMessage() const noexcept;

   void SetSortFile(const ASortFile& sortFile) noexcept;
   void SetSortFile(const ASortFile&& sortFile) noexcept;

   static bool CanTagFile(const File& file) noexcept;

private:
   ASortFile fSortFile;
   String fLastError;
};
