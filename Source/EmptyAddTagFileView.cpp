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

#include "EmptyAddTagFileView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AEmptyAddTagFileView::AEmptyAddTagFileView ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]


    //[UserPreSize]
    //[/UserPreSize]

    setSize (340, 467);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

AEmptyAddTagFileView::~AEmptyAddTagFileView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AEmptyAddTagFileView::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    {
        int x = 0, y = 0, width = getWidth() - 0, height = getHeight() - 0;
        Colour strokeColour = Colour (0xffb4b8b5);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (strokeColour);
        g.drawRect (x, y, width, height, 5);

    }

    {
        int x = 28, y = 100, width = proportionOfWidth (0.7996f), height = 26;
        String text (TRANS("Add 100 or more examples "));
        Colour fillColour = Colour (0xffb4b8b5);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (Font::getDefaultSansSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::topLeft, true);
    }

    {
        int x = 28, y = 52, width = proportionOfWidth (0.8000f), height = 16;
        String text (TRANS("This tag doesn\'t have any files."));
        Colour fillColour = Colour (0xffb4b8b5);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (Font::getDefaultSansSerifFontName(), 18.00f, Font::italic));
        g.drawText (text, x, y, width, height,
                    Justification::bottomLeft, true);
    }

    {
        int x = 28, y = 124, width = proportionOfWidth (0.7995f), height = 30;
        String text (TRANS("to help train this tag."));
        Colour fillColour = Colour (0xffb4b8b5);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (Font::getDefaultSansSerifFontName(), 21.00f, Font::plain).withTypefaceStyle ("Bold"));
        g.drawText (text, x, y, width, height,
                    Justification::topLeft, true);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AEmptyAddTagFileView::resized()
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

<JUCER_COMPONENT documentType="Component" className="AEmptyAddTagFileView" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="340" initialHeight="467">
  <BACKGROUND backgroundColour="323e44">
    <RECT pos="0 0 0M 0M" fill="solid: 666666" hasStroke="1" stroke="5, mitered, butt"
          strokeColour="solid: ffb4b8b5"/>
    <TEXT pos="28 100 79.955% 26" fill="solid: ffb4b8b5" hasStroke="0"
          text="Add 100 or more examples " fontname="Default sans-serif font"
          fontsize="21.0" kerning="0.0" bold="1" italic="0" justification="9"
          typefaceStyle="Bold"/>
    <TEXT pos="28 52 80% 16" fill="solid: ffb4b8b5" hasStroke="0" text="This tag doesn't have any files."
          fontname="Default sans-serif font" fontsize="18.0" kerning="0.0"
          bold="0" italic="1" justification="17" typefaceStyle="Italic"/>
    <TEXT pos="28 124 79.955% 30" fill="solid: ffb4b8b5" hasStroke="0"
          text="to help train this tag." fontname="Default sans-serif font"
          fontsize="21.0" kerning="0.0" bold="1" italic="0" justification="9"
          typefaceStyle="Bold"/>
  </BACKGROUND>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

