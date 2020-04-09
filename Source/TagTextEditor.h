/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.3

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

#include <memory>
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"

#include "SortFile.h"
#include "Tag.h"
#include "TagsSource.h"

class ATagButton;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    \brief A control to enter tag names.

    Add a ChangeListener to this object to be informed when tags
    are tokenized.

    As typing occurs a popup menu with auto-completed tag names starting with
    the typed text appears.
 
    Before any text is entered, but when the editor has focus, the auto-complete
    contains every tag name for the sort file.

    Selecting one of the completions from the menu results in tokenizing of the
    tag as a ATagButton and added to this component.

    Entering white space causes the text entered to be matched against
    possible tag names. If there is a match, the tag is tokenized. If not,
    the error is indicated, and the focus is left on the text.

    When the tag is tokenized, a ChangeMessage is sent to listeners.
    When a tag is removed, a ChangeMessage is sent to listeners.

                                                                    //[/Comments]
*/
class ATagTextEditor  : public Component,
                        public TextEditor::Listener,
                        public Button::Listener,
                        public ChangeBroadcaster
{
public:
    //==============================================================================
    ATagTextEditor ();
    ~ATagTextEditor();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

   /**
    * The sortfile determines which tags are available.
    * This does not clear the current tags.
    */
   void SetSortFile(const ASortFile& sortFile);

   /** The entered tags, not including any incomplete names. */
   const Tags& CurrentTags() const;

   /** Clear any text and remove all tag buttons */
   void Clear();

   // Mark:: TextEditor::Listener API, for tokenizing tag names
   void textEditorTextChanged(TextEditor&) override;
   void textEditorReturnKeyPressed(TextEditor&) override;

   // Mark:: Button::Listener API
   // No function is performed on click outside the tag button's close area.
   void buttonClicked(Button* button) override;
   
   /** Show the auto-complete menu */
   void focusGained(FocusChangeType cause) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentSizeChanged() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

   /**
    * Layout tag tokens and texteditor in this component, putting tokens first.
    */
   void Layout();
   
   /**
    * Given the text editor's text add items for tag names that complete it.
    * If te's text is empty, add all tag names.
    */
   void PopulateCompletionPopup(String& text);
   
   /** \return true of the text could be completed and tokenized, false otherwise */
   bool TryToComplete(String text); // modifies text to do its work
   
   /** Asychronously show the completion popup and try to tokenize the user's selection. */
   void ShowCompletionPopup();

   /**
    * Tokenize the tag with the given classId if any match.
    */
   bool TokenizeTagMatching(int classId);

   /**
    * Tokenize the tag with the given name if any match.
    */
   bool TokenizeTagMatching(const String& name);

   /**
    * Create a button for the given tag if the tag is not already a token.
    */
   void TokenizeTag(const ATag& tag);

   /**
    * Delete all tags and buttons representing the tokens.
    */
   void ClearTokens();

   /**
    * Remove the child button from the hierarchy and delete it, erase the
    * corresponding tag in fTags,
    */
   void RemoveToken(ATagButton* button);
   
   /**
    * Expands text to the first tag name that begins with it. text is unchanged
    * if it does not match any tag name.
    */
   void CompleteTagName(String& text) const;

   ATagsSource fTagsSource;
   std::unique_ptr<juce::PopupMenu> fCompletionPopup;
   Tags fTags;
   std::vector<ATagButton*> fTagButtons;

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<TextEditor> fTextEditor;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATagTextEditor)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

