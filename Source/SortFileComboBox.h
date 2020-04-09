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

#include "../JuceLibraryCode/JuceHeader.h"

#include "SortFilesSource.h"

/**
 * A combo box that provides a list of the available sort files preceeded
 * by one item to create a new sort file.
 */
class SortFileComboBox : public ComboBox
{
public:
   enum class InitialText { kChoose, kCreate };
   explicit SortFileComboBox(InitialText initialText = InitialText::kCreate);
   
   void paint (Graphics&) override;
   
   ASortFile SelectedSortFile() const;

   int CreateItemId() const noexcept;
   int FirstSortItemId() const noexcept;
   
   /** Rebuild the menu.
    * \param refreshSource If this is true, the sort file source rereads the list of files.
    */
   void Populate(bool refreshSource = false);
   
private:
   
   ASortFilesSource fSource;
   const int kCreateItemId;
   
   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SortFileComboBox)
};
