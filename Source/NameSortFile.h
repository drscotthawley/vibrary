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

class ATextEditLookAndFeel;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    If run modally, returns 1 if user click okay, 0 if cancel/esc/close button.

                                                                    //[/Comments]
*/
class ANameSortFile  : public Component,
                       public Button::Listener
{
public:
    //==============================================================================
    ANameSortFile ();
    ~ANameSortFile();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

   enum Result { kCancelled, kOkay };
   
   String Name() const { return fNewNameText->getText(); }
   void SetName(const String& name) { fNewNameText->setText(name); }

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
   std::unique_ptr<ATextEditLookAndFeel> fLookAndFeel;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<TextEditor> fNewNameText;
    std::unique_ptr<TextButton> fOkButton;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ANameSortFile)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

