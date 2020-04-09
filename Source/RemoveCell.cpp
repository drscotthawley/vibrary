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

#include "SimpleFileTable.h"

//[/Headers]

#include "RemoveCell.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ARemoveCell::ARemoveCell ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fButton.reset (new ImageButton ("button"));
    addAndMakeVisible (fButton.get());
    fButton->setButtonText (String());

    fButton->setImages (false, true, true,
                        ImageCache::getFromMemory (BinaryData::icondeletetag_png, BinaryData::icondeletetag_pngSize), 1.000f, Colour (0x00000000),
                        ImageCache::getFromMemory (BinaryData::icondeletetag_png, BinaryData::icondeletetag_pngSize), 1.000f, Colour (0x00000000),
                        ImageCache::getFromMemory (BinaryData::icondeletetag_png, BinaryData::icondeletetag_pngSize), 1.000f, Colour (0x00000000));

    //[UserPreSize]
    //[/UserPreSize]

    setSize (50, 18);


    //[Constructor] You can add your own custom stuff here..

   fButton->onClick = [this]{ fOwner->RemoveItem(fRow); };

    //[/Constructor]
}

ARemoveCell::~ARemoveCell()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ARemoveCell::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ARemoveCell::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fButton->setBounds ((getWidth() / 2) - (12 / 2), (getHeight() / 2) - (12 / 2), 12, 12);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

ARemoveCell::ARemoveCell(ASimpleFileTable* owner)
: ARemoveCell()
{
   fOwner = owner;
}

void ARemoveCell::SetRowAndColumn(int row, int column)
{
   fRow = row;
   fColumn = column;
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ARemoveCell" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="50" initialHeight="18">
  <BACKGROUND backgroundColour="323e44"/>
  <IMAGEBUTTON name="button" id="50fd1a3cb877b298" memberName="fButton" virtualName=""
               explicitFocusOrder="0" pos="0Cc 0Cc 12 12" buttonText="" connectedEdges="0"
               needsCallback="0" radioGroupId="0" keepProportions="1" resourceNormal="BinaryData::icondeletetag_png"
               opacityNormal="1" colourNormal="0" resourceOver="BinaryData::icondeletetag_png"
               opacityOver="1" colourOver="0" resourceDown="BinaryData::icondeletetag_png"
               opacityDown="1" colourDown="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

