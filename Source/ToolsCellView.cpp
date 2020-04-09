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

#include "ToolsCellView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AToolsCellView::AToolsCellView ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fPlayButton.reset (new ImageButton ("play button"));
    addAndMakeVisible (fPlayButton.get());
    fPlayButton->setButtonText (TRANS("new button"));

    fPlayButton->setImages (false, true, true,
                            ImageCache::getFromMemory (BinaryData::iconplay_png, BinaryData::iconplay_pngSize), 1.000f, Colour (0x00000000),
                            ImageCache::getFromMemory (BinaryData::iconplay_png, BinaryData::iconplay_pngSize), 1.000f, Colour (0x00000000),
                            ImageCache::getFromMemory (BinaryData::iconplay_png, BinaryData::iconplay_pngSize), 1.000f, Colour (0x00000000));
    fPlayButton->setBounds (0, 0, 20, 20);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (64, 20);


    //[Constructor] You can add your own custom stuff here..

   fPlayButton->onClick = [this]()
    {
       if (nullptr != fDelegate)
       {
          fDelegate->postCommandMessage(Files::kPreviewToggle);
       }
    };

    //[/Constructor]
}

AToolsCellView::~AToolsCellView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fPlayButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AToolsCellView::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AToolsCellView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..

   fPlayButton->centreWithSize(20, 20);
   auto pos = fPlayButton->getPosition();
   fPlayButton->setTopLeftPosition(0, pos.y);
   
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void AToolsCellView::SetDelegate(Component* delegate)
{
   fDelegate = delegate;
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AToolsCellView" componentName=""
                 parentClasses="public Component, public ChangeBroadcaster" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="64" initialHeight="20">
  <BACKGROUND backgroundColour="ffffff"/>
  <IMAGEBUTTON name="play button" id="df1205ff14ccc499" memberName="fPlayButton"
               virtualName="" explicitFocusOrder="0" pos="0 0 20 20" buttonText="new button"
               connectedEdges="0" needsCallback="0" radioGroupId="0" keepProportions="1"
               resourceNormal="BinaryData::iconplay_png" opacityNormal="1.0"
               colourNormal="0" resourceOver="BinaryData::iconplay_png" opacityOver="1.0"
               colourOver="0" resourceDown="BinaryData::iconplay_png" opacityDown="1.0"
               colourDown="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

