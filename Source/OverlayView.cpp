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

#include "CommandMessages.h"

//[/Headers]

#include "OverlayView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AOverlayView::AOverlayView ()
{
    //[Constructor_pre] You can add your own custom stuff here..
   fCenterIn = nullptr;
   fOwner = nullptr;
   this->setAlwaysOnTop(true);
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

AOverlayView::~AOverlayView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AOverlayView::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0x66030303));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AOverlayView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AOverlayView::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
   this->setSize(fCenterIn->getWidth(), fCenterIn->getHeight());
    //[/UserCode_parentSizeChanged]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void AOverlayView::handleCommandMessage(int commandId)
{
   if (Overlay::kExit == commandId || Overlay::kAccepted == commandId)
   {
      this->Exit(commandId);
   }
}


void AOverlayView::Exit(int command)
{
   auto parent = this->getParentComponent();
   parent->removeChildComponent(this);
   if (nullptr != fOwner)
   {
      fOwner->postCommandMessage(command);
      fOwner = nullptr;
      fCenterIn = nullptr;
   }
}


void AOverlayView::ShowIn(Component* owner, Component* centerInComponent/*=nullptr*/)
{
   jassert(nullptr != owner && "OverlayView needs a parent component");

   fOwner = owner;
   fCenterIn = centerInComponent;
   if (nullptr == centerInComponent)
   {
      fCenterIn = owner;
   }
   fCenterIn->addAndMakeVisible(this);

   this->setSize(fCenterIn->getWidth(), fCenterIn->getHeight());
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AOverlayView" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="66030303"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

