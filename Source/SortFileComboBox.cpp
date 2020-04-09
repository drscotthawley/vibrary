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

#include "SortFileComboBox.h"
#include "SortFilesSource.h"


namespace
{
   constexpr auto kChooseSortFileText = "Choose A Sort File";
   constexpr auto kCreateSortFileText = "Create New Sort File";
}


SortFileComboBox::SortFileComboBox(InitialText initialText/*=kCreate*/)
: kCreateItemId(1)
{
   // These were in the Projucer generated intialization before we used this
   // subclass.
   this->setEditableText(false);
   this->setJustificationType(Justification::centredLeft);
   if (InitialText::kChoose == initialText)
   {
      this->setTextWhenNothingSelected(TRANS(kChooseSortFileText));
      this->setTextWhenNoChoicesAvailable(TRANS(kChooseSortFileText));
   }
   else
   {
      this->setTextWhenNothingSelected(TRANS(kCreateSortFileText));
      this->setTextWhenNoChoicesAvailable(TRANS(kCreateSortFileText));
   }
   
   this->Populate();
}


void SortFileComboBox::paint (Graphics& g)
{
   ComboBox::paint(g);
}


int SortFileComboBox::CreateItemId() const noexcept
{
   return kCreateItemId;
}


int SortFileComboBox::FirstSortItemId() const noexcept
{
   return this->CreateItemId() + 1;
}


void SortFileComboBox::Populate(bool refresh/*=false*/)
{
   if (refresh)
   {
      fSource = ASortFilesSource();
   }

   auto selected = this->SelectedSortFile();
   auto selectedId = this->getSelectedId();

   this->clear();

   this->addItem(TRANS(kCreateSortFileText), this->CreateItemId());

   const int firstSortItemId = this->FirstSortItemId();
   for (size_t i = 0; i < fSource.Size(); ++i)
   {
      const int itemId = static_cast<int>(i + firstSortItemId);
      this->addItem(fSource[i].Name(), itemId);

      if (fSource[i].Key() == selected.Key())
      {
         selectedId = itemId;
      }
   }

   if (this->CreateItemId() == selectedId)
   {
      selectedId = 0;
   }
   
   if (this->getSelectedId() != selectedId && 0 != selectedId)
   {
      this->setSelectedId(selectedId);
   }
}


ASortFile SortFileComboBox::SelectedSortFile() const
{
   // Account for the initial create sort file item, and that JUCE doesn't allow
   // an item id of 0.
   const auto index = this->getSelectedId() - this->FirstSortItemId();
   return (index < fSource.Size() ? fSource[index] : ASortFile());
}
