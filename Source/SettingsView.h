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

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SettingsView  : public ContentView,
                      public TextEditor::Listener
{
public:
    //==============================================================================
    SettingsView ();
    ~SettingsView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

   /** Save all settings in their current values. */
   void LeavingView() override;

   void textEditorFocusLost(TextEditor& textEditor) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentSizeChanged() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

   void PopulateWithSettings(NotificationType notify);

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<TextEditor> fUsername;
    std::unique_ptr<TextEditor> fServer;
    std::unique_ptr<Drawable> drawable1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SettingsView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

