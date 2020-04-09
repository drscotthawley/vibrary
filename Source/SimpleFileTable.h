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

#include <memory>

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
/*
*/
class ASimpleFileTable : public Component, public TableListBoxModel
{
public:
    ASimpleFileTable();
    ~ASimpleFileTable();

   void AddFiles(const Array<File>& files);
   void RemoveItem(int row);
   void Clear();
   
   constexpr const Array<File>& Files() const noexcept { return fFiles; }
   constexpr int FileCount() const noexcept { return fCachedFileCount; }
   
   using RemoveCallback = std::function<void(void)>;
   void SetRemoveCallback(RemoveCallback cb);
   
   // Model interface
   int getNumRows() override;
   void paintRowBackground(Graphics& g, int rowNumber, int width, int height,
                           bool rowIsSelected) override;
   void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height,
                  bool rowIsSelected) override;
   Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                      Component* existingComponentToUpdate) override;
   void deleteKeyPressed(int lastRowSelected) override;
   
   // Component interface
   void resized() override;

private:
   enum ColumnIds: int { kName = 1, kSize, kRemove };
   
   TableListBox fTable { "file list", this };
   Font fFont { Font::getDefaultSansSerifFontName(), 14.0f, Font::plain };

   RemoveCallback fRemoveCallback;
   
   Array<File> fFiles;
   int fCachedFileCount = 0;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ASimpleFileTable)
};
