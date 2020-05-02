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

#include <memory>
#include <vector>

#include "../JuceLibraryCode/JuceHeader.h"

#include "Tag.h"

class AButtonLookAndFeel;
class ASortFile;
class ATagButton;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
 Displays a set of tags as buttons organized into a grid.

 Buttons representing tags call the given TagFunction.

 A special button, "Add Tags", is always present and
 posts a command message, kAddTag, to this view's parent when clicked.

                                                                    //[/Comments]
*/
class ATagView  : public Component,
                  public Button::Listener
{
public:
    //==============================================================================
    ATagView ();
    ~ATagView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

   using TagFunction = std::function<void(const ATag& tag)>;

   void AddTag(const ATag& tag);
   void SetTags(const Tags& tags);
   void SetTagsFrom(const ASortFile& sortFile);
   //constexpr size_t TagsSize() const noexcept { return fTags.size(); }
   size_t TagsSize() const noexcept { return fTags.size(); }

   /** The function to invoke when a tag button is clicked. */
   void SetTagFunction(TagFunction function);

   // MARK: ButtonListener API
   /** Finds the corresponding tag for the button and calls the tag function with it. */
   void buttonClicked(Button* buttonThatWasClicked) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

   /** Take care to call LayoutButtons() when you are done adding tag buttons */
   void AddTagButton(const ATag& tag);
   void AddButtonToAddTag();
   void ClearButtons();
   void LayoutButtons();
   void RemoveTag(const ATagButton* button);

   Tags fTags;
   std::vector<TextButton*> fButtons;
   TagFunction fTagFunction;

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<ATagButton> fAddTagButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ATagView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
