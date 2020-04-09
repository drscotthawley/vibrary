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
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An overlay displays a view centered in the centerInComponent while
    darkening the areas around its content view. Owner component is notified
    when this object is dismissed.

    Subclass to add your content to the overlay. See AAddTagsOverlay.
                                                                    //[/Comments]
*/
class AOverlayView  : public Component
{
public:
    //==============================================================================
    AOverlayView ();
    ~AOverlayView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

   /**
    * \param owner Notified when this view has exited
    * \param centerInComponent If it is nullptr, owner is used to center the overlay.
    */
   void ShowIn(Component* owner, Component* centerInComponent = nullptr);

   /** Handles kExit command by calling Exit. */
   void handleCommandMessage(int commandId) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void parentSizeChanged() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.

protected:

   /** Removes this component from parent, and sends command to parent (see TxCommands. */
   virtual void Exit(int command);

private:
   Component* fOwner;    //!< receives command this object was dismissed.
   Component* fCenterIn; //!< Cover and center content in this view.

    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AOverlayView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

