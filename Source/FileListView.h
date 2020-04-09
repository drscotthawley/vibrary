/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.4

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
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

#include "ContentView.h"
#include "TaggingManager.h"

class AButtonLookAndFeel;
class ADetailFileTable;
class AEmptyFileListView;
class AFileToolBar;
class AProgressOverlay;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class FileListView  : public ContentView,
                      public ChangeListener,
                      public FileDragAndDropTarget,
                      public Button::Listener
{
public:
    //==============================================================================
    FileListView ();
    ~FileListView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

   // MARK: ChangeListener API
   /** Listens for messages from the toolbar to trigger reloads of the file list. */
   void changeListenerCallback(ChangeBroadcaster* source) override;

   // MARK: DragAndDropTarget API
   bool isInterestedInFileDrag(const StringArray& files) override;

   void handleCommandMessage(int commandId) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void parentSizeChanged() override;
    void filesDropped (const StringArray& filenames, int mouseX, int mouseY) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

   /** Update the table, use after changes made to files in db or search params */
   void ReloadFileTable(bool clearSelection = false);

   /** Hide the table and display a help message for the user. */
   void HandleEmptyFileTable();

   void ChooseAndAddFiles();
   void AddFiles(const Array<File>& files);
   void RemoveSelectedFiles();

   ATaggingManager fTaggingManager;
   std::unique_ptr<AButtonLookAndFeel> fButtonLookAndFeel;
   std::unique_ptr<AProgressOverlay> fProgressOverlay;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<AEmptyFileListView> fEmptyView;
    std::unique_ptr<ADetailFileTable> fFileTable;
    std::unique_ptr<TextButton> fAddFilesButton;
    std::unique_ptr<AFileToolBar> fToolBar;
    std::unique_ptr<TextButton> fRemoveButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileListView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

