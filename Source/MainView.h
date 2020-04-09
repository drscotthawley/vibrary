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

class ContentView;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class MainView  : public Component,
                  public Button::Listener
{
public:
    //==============================================================================
    MainView ();
    ~MainView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

   void handleCommandMessage(int commandId) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;

    // Binary resources:
    static const char* naviconfolder_png;
    static const int naviconfolder_pngSize;
    static const char* naviconfolderselected_png;
    static const int naviconfolderselected_pngSize;
    static const char* navicontrainselected_png;
    static const int navicontrainselected_pngSize;
    static const char* navicontrain_png;
    static const int navicontrain_pngSize;
    static const char* naviconsettings_png;
    static const int naviconsettings_pngSize;
    static const char* naviconsettingsselected_png;
    static const int naviconsettingsselected_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.

   enum ContentIds { kFileListView, kTrainingView, kSettingsView };

   /** Let the current view know that it is going away. */
   void PrepareToLeaveView();

   /** Lazily create contents and show in fContentView. Update fContentId. */
   void ShowContents(ContentIds content);
   void SendSortFileAdded() const;
   ContentView* Contents(ContentIds contentId) const;

   std::array<std::unique_ptr<Component>,3> fContents;
   ContentIds fContentId = kFileListView;

    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Component> fContentView;
    std::unique_ptr<ImageButton> fSortButton;
    std::unique_ptr<ImageButton> fSettingsButton;
    std::unique_ptr<ImageButton> fTrainButton;
    std::unique_ptr<Drawable> drawable1;
    std::unique_ptr<Drawable> drawable2;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

