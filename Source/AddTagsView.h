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

#include "AddTagInfoView.h"
#include "ButtonLookAndFeel.h"
#include "CommandMessages.h"
#include "EmptyAddTagFileView.h"
#include "SimpleFileTable.h"

class ATag;
class ATextEditLookAndFeel;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
   Ask the user to define examples for a tag.

   You probably don't want to use this directly. See AddTagsOverlay.
                                                                    //[/Comments]
*/
class AAddTagsView  : public Component,
                      public FileDragAndDropTarget,
                      public TextEditor::Listener,
                      public Button::Listener
{
public:
    //==============================================================================
    AAddTagsView ();
    ~AAddTagsView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

   // The data the user entered.
   String TagName() const;
   const Array<File>& Files() const;

   /** Erase any entered values from a previous invocation. */
   void Clear();

   /** Initialize the file list to some pre-selected files. */
   void SetFiles(const Array<File>& files);
   /** Initialize the file list to some pre-selected files. */
   void SetFiles(const Array<File>&& files);

   void SetTagInfo(const ATag& tag);

   // MARK: DragAndDropTarget API
   bool isInterestedInFileDrag(const StringArray& files) final;
   void filesDropped(const StringArray& filenames, int mouseX, int mouseY) final;

   // MARK: TextEditor::Listener API
   void textEditorTextChanged(TextEditor& textEditor) final;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    bool keyPressed (const KeyPress& key) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
   void ChooseAndAddFiles();

   /** Inform the file table of new files and manage related GUI changes */
   void AddFiles(const Array<File>& files);

   bool AskToChangeTagNameTo(const String& name) const;

   void HandleEmptyFileList();

   /** Restore initial values for all controls. */
   void ResetGui();
   void UpdateGui(int fileCount);
   void UpdateSaveButton();

   /** Posts kExit command to parent. */
   void Exit(AddTags::Command command);

   std::unique_ptr<AButtonLookAndFeel> fButtonLookAndFeel;
   std::unique_ptr<ATextEditLookAndFeel> fTextEditLookAndFeel;

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<ASimpleFileTable> fFileTable;
    std::unique_ptr<AEmptyAddTagFileView> fEmptyFileView;
    std::unique_ptr<Label> fAlsoDragLabel;
    std::unique_ptr<Label> fTagNameLabel;
    std::unique_ptr<AAddTagInfoView> fTagInfoView;
    std::unique_ptr<Label> fTagsLabel;
    std::unique_ptr<TextEditor> fTagNameTextBox;
    std::unique_ptr<ToggleButton> fDisableTagButton;
    std::unique_ptr<TextButton> fSaveButton;
    std::unique_ptr<TextButton> fAddButton;
    std::unique_ptr<TextButton> fDeleteButton;
    std::unique_ptr<TextButton> fAddMoreFilesButton;
    std::unique_ptr<TextEditor> fAddExamplesText;
    std::unique_ptr<TextButton> fCancelButton;
    std::unique_ptr<Drawable> drawable1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AAddTagsView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

