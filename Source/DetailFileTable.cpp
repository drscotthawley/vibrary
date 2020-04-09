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

#include <algorithm>
#include <iterator>
#include <sstream>

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppLookAndFeel.h"
#include "CommandMessages.h"
#include "DetailFileTable.h"
#include "HeaderLookAndFeel.h"
#include "ToolsCellView.h"
#include "StringUtils.h"

namespace
{
   /** Convert tags to a stream of comma separated string of tag names. */
   template <class Stream>
   Stream& operator<<(Stream& s, const Tags& tags);

   constexpr float kHeaderFontSizePixels = 18.0f;
   constexpr float kNameAndSizeFontSizePixels = 17.0f;
   constexpr int kTableRowHeight = 28;
}


const Colour ADetailFileTable::kNormalColour = Colour(0xFFEBEBEB);


ADetailFileTable::ADetailFileTable()
: fCachedFileCount(0L)
, fHeaderLookAndFeel(new AHeaderLookAndFeel(kHeaderFontSizePixels))
, fToolsCell(nullptr)
{
   this->addAndMakeVisible(fTable);
   fTable.setOutlineThickness(0);
   fTable.setMultipleSelectionEnabled(true);
   fTable.setColour(ListBox::backgroundColourId, kNormalColour);
   fTable.setRowHeight(kTableRowHeight);

   fHeaderLookAndFeel->fJustifications[kSize] = Justification::centredRight;
   fHeaderLookAndFeel->setColour(TableHeaderComponent::backgroundColourId, kNormalColour);
   fHeaderLookAndFeel->setColour(TableHeaderComponent::textColourId, Colour(0xFFB4B8B5));
   fHeaderLookAndFeel->setColour(TableHeaderComponent::outlineColourId, Colour(0x00000000));

   auto& header = fTable.getHeader();
   header.setLookAndFeel(fHeaderLookAndFeel.get());
   header.setSize(1, 50);

   const auto flagVisible = TableHeaderComponent::visible;
   const auto flagSortable = flagVisible | TableHeaderComponent::sortable;
   header.addColumn(TRANS("Name"), kName, 270, 270, -1, flagSortable);
   header.addColumn(TRANS("Size"), kSize, 80, 80, 80, flagSortable);
   header.addColumn(TRANS("Tags"), kTags, 380, 380, -1, flagVisible);
   header.addColumn({""}, kTools, 60, 60, 60, flagVisible);
   header.setSortColumnId(kName, true);

   this->setAudioChannels(0, 2);
}


ADetailFileTable::~ADetailFileTable()
{
   fTable.getHeader().setLookAndFeel(nullptr);
   
   fPlayer.Stop();
   this->shutdownAudio();

   fToolsCell = nullptr;
}


void ADetailFileTable::LoadFiles(int64_t sortFileId, const Tags& tags, bool clearSelection)
{
   fCachedFileCount = fFiles.LoadFiles(sortFileId, tags);
   if (clearSelection)
   {
      fTable.selectRow(-1);
   }
   fTable.updateContent();
   fTable.repaint();
}


void ADetailFileTable::PrepareToPlayFileAtIndex(int rowNumber)
{
   jassert(rowNumber >= 0);
   const auto& file = fFiles[rowNumber].file;
   if (file.exists())
   {
      fPlayer.SetAudioFile(file);
   }
   else
   {
      // TODO: Report errors or propagate to app
      std::cerr << "Cannot find audio file " << file.getFullPathName().toStdString() << "\n";
   }
}


File ADetailFileTable::SelectedFile() const
{
   return fFiles[fTable.getSelectedRow()].file;
}


StringArray ADetailFileTable::SelectedFiles() const
{
   auto rows = fTable.getSelectedRows();

   StringArray files;
   for (int i = 0; i < rows.size(); ++i)
   {
      files.add(fFiles[rows[i]].Path());
   }
   return files;
}


std::vector<int64_t> ADetailFileTable::SelectedFileKeys() const
{
   auto rows = fTable.getSelectedRows();

   std::vector<int64_t> keys;
   keys.reserve(rows.size());
   for (int i = 0; i < rows.size(); ++i)
   {
      keys.push_back(fFiles[rows[i]].key);
   }
   return keys;
}


size_t ADetailFileTable::SelectionSize() const
{
   return fTable.getSelectedRows().size();
}


void ADetailFileTable::ToggleAudioPlaying()
{
   constexpr bool kSendChange = true;

   if (fPlayer.IsPlaying())
   {
      fPlayer.Stop(kSendChange);
   }
   else
   {
      fPlayer.Play(kSendChange);
   }
}


// MARK: Drag And Drop Support

var ADetailFileTable::getDragSourceDescription(const SparseSet<int>& currentlySelectedRows)
{
   return var("Audio Files");
}


bool ADetailFileTable::shouldDropFilesWhenDraggedExternally(const DragAndDropTarget::SourceDetails& sourceDetails,
                                                            StringArray& files,
                                                            bool& canMoveFiles)
{
   files = this->SelectedFiles();
   return true;
}


bool ADetailFileTable::shouldDropTextWhenDraggedExternally(const DragAndDropTarget::SourceDetails& sourceDetails,
                                                           String& text)
{
   text = this->SelectedFile().getFullPathName();
   return true;
}


// MARK: TableListBoxModel API

void ADetailFileTable::cellClicked(int rowNumber, int columnId, const MouseEvent&)
{
   this->PrepareToPlayFileAtIndex(rowNumber);
}


int ADetailFileTable::getNumRows()
{
   int count = static_cast<int>(fCachedFileCount);
   if (count != fCachedFileCount)
   {
      // !!!:
   }
   return count;
}


void ADetailFileTable::paintRowBackground(Graphics& g, int /*rowNumber*/,
                                          int width, int height,
                                          bool rowIsSelected)
{
   g.setColour(rowIsSelected ? Colours::white : kNormalColour);
   g.fillRect(0, 0, width, height);
}


void ADetailFileTable::paintCell(Graphics& g, int rowNumber, int columnId,
                                 int width, int height, bool rowIsSelected)
{
   jassert(rowNumber < this->getNumRows());
   
   g.setColour(rowIsSelected ? Colours::darkblue : getLookAndFeel().findColour(ListBox::textColourId));

   // Common drawing spacing
   constexpr int kBorder = 2;

   auto info = fFiles[rowNumber];
   if (kName == columnId)
   {
      constexpr int kThumbWidth = 27;
      constexpr int kThumbHeight = 17;
      const int border = (height - kThumbHeight) / 2;

      auto& thumb = info.thumbnail;
      g.setOpacity(1.0f);
      g.drawImageAt(thumb, border, border, false);

      const int nameX = border * 2 + kThumbWidth;
      const int nameWidth = width - kThumbWidth;

      fFont.setHeight(kNameAndSizeFontSizePixels);
      fFont.setUnderline(true);
      g.setFont(fFont);

      g.drawText(info.Name(),
                 nameX, border, nameWidth, height - border * 2,
                 Justification::centredLeft, true);
   }
   else if (kSize == columnId)
   {
      fFont.setHeight(kNameAndSizeFontSizePixels);
      fFont.setUnderline(false);
      g.setFont(fFont);
      
      // TODO: Format size as human readable, e.g. 128Mb instead of 127895099.
      auto size = String(StringUtils::HumanBytes(info.Size()).c_str());
      g.drawText(size,
                 kBorder, kBorder, width - kBorder * 2, height - kBorder * 2,
                 Justification::centredRight, true);
   }
   else if (kTags == columnId && !info.tags.empty())
   {
      constexpr auto kTagBorder = kBorder + 8;
      std::stringstream tagsSs;
      tagsSs << info.tags;
      g.drawText(tagsSs.str().c_str(),
                 kTagBorder, kBorder, width - kTagBorder * 2, height - kBorder * 2,
                 Justification::centredLeft, true);
   }
}


Component* ADetailFileTable::refreshComponentForCell(int rowNumber, int columnId,
                                                     bool isRowSelected,
                                                     Component* existingComponentToUpdate)
{
   if (kTools == columnId)
   {
      if (isRowSelected)
      {
         if (nullptr == existingComponentToUpdate)
         {
            fToolsCell = new AToolsCellView();
            fToolsCell->SetDelegate(this);
            existingComponentToUpdate = fToolsCell;
         }
      }
      else if (nullptr != existingComponentToUpdate)
      {
         if (fToolsCell == existingComponentToUpdate)
         {
            fToolsCell = nullptr;
         }
         deleteAndZero(existingComponentToUpdate);
      }
      return existingComponentToUpdate;
   }
   return nullptr;
}


void ADetailFileTable::selectedRowsChanged(int lastRowSelected)
{
   if (lastRowSelected > 0)
   {
      this->PrepareToPlayFileAtIndex(lastRowSelected);
   }
   this->sendChangeMessage();
}


// MARK: - AudioAppComponent API


void ADetailFileTable::getNextAudioBlock(const AudioSourceChannelInfo& ci)
{
   fPlayer.GetNextBlock(ci);
}


void ADetailFileTable::prepareToPlay(int numSamples, double sampleRate)
{
   fPlayer.Prepare(numSamples, sampleRate);
}


void ADetailFileTable::releaseResources()
{
   fPlayer.ReleaseResources();
}


// MARK: - Component API

void ADetailFileTable::handleCommandMessage(int commandId)
{
   if (Files::kPreviewToggle == commandId)
   {
      this->ToggleAudioPlaying();
   }
}


bool ADetailFileTable::keyPressed(const KeyPress& kp)
{
   bool handled = false;
   if (kp.getKeyCode() == KeyPress::spaceKey)
   {
      this->ToggleAudioPlaying();
      handled = true;
   }
   else if (kp.getKeyCode() == KeyPress::leftKey)
   {
      fPlayer.SeekToStart();
      handled = true;
   }
   return handled;
}


void ADetailFileTable::resized()
{
   fTable.setBoundsInset(BorderSize<int>(0));
}


// MARK: Static functions

namespace
{
   /** Convert tags to a stream of comma separated string tag names. */
   template <class Stream>
   Stream& operator<<(Stream& s, const Tags& tags)
   {
      if (tags.empty())
      {
         return s;
      }

      auto first = tags.begin();
      auto last = tags.begin() + tags.size() - 1;

      std::transform(first, last, std::ostream_iterator<String>(s, ","),
       [](const ATag& tag)
       {
          return tag.Name();
       });
      s << last->Name();
      return s;
   }
}
