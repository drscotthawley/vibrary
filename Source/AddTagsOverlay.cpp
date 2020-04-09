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

//[Headers] You can add your own extra header files here...
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
//[/Headers]

#include "AddTagsOverlay.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AAddTagsOverlay::AAddTagsOverlay ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fAddTagsView.reset (new AAddTagsView());
    addAndMakeVisible (fAddTagsView.get());
    fAddTagsView->setExplicitFocusOrder (1);
    fAddTagsView->setName ("add tags view");


    //[UserPreSize]
    //[/UserPreSize]

    setSize (683, 500);


    //[Constructor] You can add your own custom stuff here..
   fAddTagsView->setFocusContainer(true);
    //[/Constructor]
}

AAddTagsOverlay::~AAddTagsOverlay()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fAddTagsView = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AAddTagsOverlay::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
   AOverlayView::paint(g);
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AAddTagsOverlay::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fAddTagsView->setBounds ((getWidth() / 2) - (683 / 2), (getHeight() / 2) - (500 / 2), 683, 500);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

bool AAddTagsOverlay::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
   bool handled = false;
   if (key.isKeyCode(KeyPress::escapeKey))
   {
      handled = true;
      this->Exit(Overlay::kDismissed);
   }
   return handled;
    //[/UserCode_keyPressed]
}

void AAddTagsOverlay::focusGained (FocusChangeType cause)
{
    //[UserCode_focusGained] -- Add your code here...

   // Transfer the focus to the content.
   fAddTagsView->grabKeyboardFocus();

    //[/UserCode_focusGained]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void AAddTagsOverlay::Clear()
{
   fAddTagsView->Clear();
}

const Array<File>& AAddTagsOverlay::Files() const
{
   return fAddTagsView->Files();
}

void AAddTagsOverlay::SetFiles(const Array<File>& files)
{
   fAddTagsView->SetFiles(files);
}

void AAddTagsOverlay::SetFiles(const Array<File>&& files)
{
   fAddTagsView->SetFiles(std::move(files));
}

void AAddTagsOverlay::SetTagInfo(const ATag& tag)
{
   fAddTagsView->SetTagInfo(tag);
}


String AAddTagsOverlay::TagName() const
{
   return fAddTagsView->TagName();
}


void AAddTagsOverlay::handleCommandMessage(int commandId)
{
   if (AddTags::kSave == commandId)
   {
      this->Exit(Overlay::kAccepted);
   }
   else if (AddTags::kCancel == commandId)
   {
      this->Exit(Overlay::kDismissed);
   }
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AAddTagsOverlay" componentName=""
                 parentClasses="public AOverlayView" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="683" initialHeight="500">
  <METHODS>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
    <METHOD name="focusGained (FocusChangeType cause)"/>
  </METHODS>
  <BACKGROUND backgroundColour="323e44"/>
  <GENERICCOMPONENT name="add tags view" id="c217a9dae44a0d19" memberName="fAddTagsView"
                    virtualName="" explicitFocusOrder="1" pos="0.5Cc 0Cc 683 500"
                    class="AAddTagsView" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

