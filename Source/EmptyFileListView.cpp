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
//[/Headers]
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

#include "EmptyFileListView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AEmptyFileListView::AEmptyFileListView ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 151);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

AEmptyFileListView::~AEmptyFileListView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AEmptyFileListView::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffcccccc));

    {
        int x = 12, y = 12, width = getWidth() - 24, height = getHeight() - 24;
        Colour strokeColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 8);

    }

    {
        int x = (getWidth() / 2) - (proportionOfWidth (1.0000f) / 2), y = 60, width = proportionOfWidth (1.0000f), height = 60;
        String text (TRANS("Your vibrary is empty!"));
        Colour fillColour = Colour (0xff6988e0);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (Font::getDefaultSansSerifFontName(), 48.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = (getWidth() / 2) - (proportionOfWidth (1.0000f) / 2), y = getHeight() - 270, width = proportionOfWidth (1.0000f), height = 30;
        String text (TRANS("You haven\'t added any audio files yet."));
        Colour fillColour = Colour (0xff666666);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (Font::getDefaultSansSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = (getWidth() / 2) - (proportionOfWidth (1.0000f) / 2), y = getHeight() - 174, width = proportionOfWidth (1.0000f), height = 28;
        String text (TRANS("or drag and drop into this area."));
        Colour fillColour = Colour (0xff666666);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (Font::getDefaultSansSerifFontName(), 28.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = (getWidth() / 2) - (proportionOfWidth (1.0000f) / 2), y = getHeight() - 214, width = proportionOfWidth (1.0000f), height = 36;
        String text (TRANS("Add files by clicking the button below"));
        Colour fillColour = Colour (0xff666666);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (Font::getDefaultSansSerifFontName(), 28.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AEmptyFileListView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AEmptyFileListView" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="151">
  <BACKGROUND backgroundColour="ffcccccc">
    <RECT pos="12 12 24M 24M" fill="solid: 2a65a5" hasStroke="1" stroke="8, mitered, butt"
          strokeColour="solid: ffffffff"/>
    <TEXT pos="0Cc 60 100% 60" fill="solid: ff6988e0" hasStroke="0" text="Your vibrary is empty!"
          fontname="Default sans-serif font" fontsize="48.0" kerning="0.0"
          bold="1" italic="0" justification="36" typefaceStyle="Bold"/>
    <TEXT pos="0Cc 270R 100% 30" fill="solid: ff666666" hasStroke="0" text="You haven't added any audio files yet."
          fontname="Default sans-serif font" fontsize="21.0" kerning="0.0"
          bold="0" italic="0" justification="36"/>
    <TEXT pos="0Cc 174R 100% 28" fill="solid: ff666666" hasStroke="0" text="or drag and drop into this area."
          fontname="Default sans-serif font" fontsize="28.0" kerning="0.0"
          bold="1" italic="0" justification="36" typefaceStyle="Bold"/>
    <TEXT pos="0Cc 214R 100% 36" fill="solid: ff666666" hasStroke="0" text="Add files by clicking the button below"
          fontname="Default sans-serif font" fontsize="28.0" kerning="0.0"
          bold="1" italic="0" justification="36" typefaceStyle="Bold"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

