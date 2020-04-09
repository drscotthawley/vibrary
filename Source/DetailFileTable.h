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
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"

#include "AppLookAndFeel.h"
#include "FileSource.h"
#include "Player.h"
#include "Tag.h"


class AHeaderLookAndFeel;
class AToolsCellView;


/**
 * Display a file, it's tags, and operations that can be done with the file.
 *
 * Also handles previewing the file.
 */
class ADetailFileTable : public AudioAppComponent,
                         public TableListBoxModel,
                         public ChangeBroadcaster,
                         public DragAndDropContainer
{
public:
    ADetailFileTable();
    ~ADetailFileTable();
   
   void LoadFiles(int64_t sortFileId, const Tags& tags, bool clearSelection);
   File SelectedFile() const;

   size_t SelectionSize() const;
   std::vector<int64_t> SelectedFileKeys() const;

     // TableListBoxModel interface
   
   void cellClicked(int rowNumber, int columnId, const MouseEvent&) override;
   int getNumRows() override;
   void handleCommandMessage(int commandId) override;
   void paintRowBackground(Graphics& g, int rowNumber, int width, int height,
                           bool rowIsSelected) override;
   void paintCell(Graphics& g, int rowNumber, int columnId, int width, int height,
                  bool rowIsSelected) override;
   Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                      Component* existingComponentToUpdate) override;
   void selectedRowsChanged(int lastRowSelected) override;


      // AudioAppComponent interface

   void getNextAudioBlock(const AudioSourceChannelInfo&) override;
   void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
   void releaseResources() override;


      // Component interface

   /** space key toggles audio playback */
   bool keyPressed(const KeyPress& press) override;
   void resized() override;

   
      // Drag and drop support

   /** \return String, "Audio Files" */
   var getDragSourceDescription(const SparseSet<int>& currentlySelectedRows) override;

   bool shouldDropFilesWhenDraggedExternally(const DragAndDropTarget::SourceDetails& sourceDetails,
                                             StringArray& files,
                                             bool& canMoveFiles) override;

   bool shouldDropTextWhenDraggedExternally(const DragAndDropTarget::SourceDetails& sourceDetails,
                                            String& text) override;

private:
   StringArray SelectedFiles() const;
   void PrepareToPlayFileAtIndex(int rowNumber);
   void ToggleAudioPlaying();

   enum ColumnIds: int { kName = 1, kSize, kTags, kTools };
   
   TableListBox fTable { "file list", this };
   Font fFont { AAppLookAndFeel::GetMontserratFace() };

   APlayer fPlayer;
   AFileSource fFiles;
   static const Colour kNormalColour;
   int64_t fCachedFileCount;
   std::unique_ptr<AHeaderLookAndFeel> fHeaderLookAndFeel;
   /// This is owned by the table, not this class. It may be deleted at any time.
   /// Keeping a pointer so we can update its state, but there may be a mechanism
   /// to force calling refreshComponentForCell where we could update it
   AToolsCellView* fToolsCell;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADetailFileTable)
};
