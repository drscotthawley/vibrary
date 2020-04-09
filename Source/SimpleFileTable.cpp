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

#include "../JuceLibraryCode/JuceHeader.h"
#include "RemoveCell.h"
#include "SimpleFileTable.h"
#include "StringUtils.h"

//==============================================================================
ASimpleFileTable::ASimpleFileTable()
{
   // In your constructor, you should add any child components, and
   // initialise any special settings that your component needs.
   
   this->addAndMakeVisible (fTable);
   fTable.setOutlineThickness(0);
   
   fTable.getHeader().addColumn({""}, kName,  170, 170, -1, TableHeaderComponent::visible);
   fTable.getHeader().addColumn({""}, kSize,   97,  97, -1, TableHeaderComponent::visible);
   fTable.getHeader().addColumn({""}, kRemove, 41,  41, 41, TableHeaderComponent::visible);
   fTable.getHeader().setSortColumnId(kName, true);
   fTable.setHeaderHeight(0);
   fTable.setMultipleSelectionEnabled(true);
}


ASimpleFileTable::~ASimpleFileTable()
{}


void ASimpleFileTable::AddFiles(const Array<File>& files)
{
   fFiles.addArray(files);
   fCachedFileCount += files.size();
   
   fTable.updateContent(); // must call on main thread
}


void ASimpleFileTable::Clear()
{
   fFiles.clear();
   fCachedFileCount = 0;
   
   fTable.updateContent(); // must call on main thread
}


void ASimpleFileTable::RemoveItem(int row)
{
   if (row >= 0 && row < fFiles.size())
   {
      fFiles.remove(row);
      fCachedFileCount -= 1;
      
      fRemoveCallback();
 
      fTable.updateContent();
   }
}

void ASimpleFileTable::SetRemoveCallback(RemoveCallback cb)
{
   fRemoveCallback = cb;
}


// MARK: TableListBoxModel interface


void ASimpleFileTable::deleteKeyPressed(int lastRowSelected)
{
   auto selections = fTable.getSelectedRows();
   const int count = selections.size();
   // Go in reverse so indices stay consistent.
   for (int row = count - 1; row >= 0; --row)
   {
      this->RemoveItem(row);
   }
}


int ASimpleFileTable::getNumRows()
{
   return fCachedFileCount;
}


void ASimpleFileTable::paintRowBackground(Graphics& /*g*/, int /*rowNumber*/,
                                          int /*width*/, int /*height*/,
                                          bool /*rowIsSelected*/)
{}


void ASimpleFileTable::paintCell(Graphics& g, int rowNumber, int columnId,
                                 int width, int height, bool rowIsSelected)
{
   jassert(rowNumber < this->getNumRows());
   
   g.setColour(rowIsSelected ? Colours::darkblue : getLookAndFeel().findColour(ListBox::textColourId));
   
   
   File& file = fFiles.getReference(rowNumber);
   if (kName == columnId)
   {
      fFont.setUnderline(true);
      g.setFont(fFont);
      
      g.drawText(file.getFileName(), 18, 2, width - 20, height - 4,
                 Justification::centredLeft, true);
   }
   else if (kSize == columnId)
   {
      fFont.setUnderline(false);
      g.setFont(fFont);
      
      auto sizeText = StringUtils::HumanBytes(file.getSize());
      g.drawText(String(sizeText), 2, 2, width - 4, height - 4,
                 Justification::centredRight, true);
   }
}


Component* ASimpleFileTable::refreshComponentForCell (int rowNumber, int columnId,
                                                      bool /*isRowSelected*/,
                                                      Component* existingComponentToUpdate)
{
   if (columnId == kRemove)
   {
      auto* removeCell = static_cast<ARemoveCell*>(existingComponentToUpdate);
      if (removeCell == nullptr)
      {
         removeCell = new ARemoveCell(this);
      }
      removeCell->SetRowAndColumn(rowNumber, columnId);
      return removeCell;
   }
   return nullptr;
}


// MARK: Component interface

void ASimpleFileTable::resized()
{
   fTable.setBoundsInset(BorderSize<int>(0));
}
