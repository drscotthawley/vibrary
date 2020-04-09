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

#include "TextEditLookAndFeel.h"

//[/Headers]

#include "NameSortFile.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
ANameSortFile::ANameSortFile ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fNewNameText.reset (new TextEditor ("new name text editor"));
    addAndMakeVisible (fNewNameText.get());
    fNewNameText->setMultiLine (false);
    fNewNameText->setReturnKeyStartsNewLine (false);
    fNewNameText->setReadOnly (false);
    fNewNameText->setScrollbarsShown (false);
    fNewNameText->setCaretVisible (true);
    fNewNameText->setPopupMenuEnabled (false);
    fNewNameText->setColour (TextEditor::textColourId, Colour (0xff666666));
    fNewNameText->setColour (TextEditor::backgroundColourId, Colour (0xffececec));
    fNewNameText->setColour (TextEditor::highlightColourId, Colour (0xbfecec65));
    fNewNameText->setColour (TextEditor::outlineColourId, Colour (0xff222222));
    fNewNameText->setText (String());

    fNewNameText->setBounds (176, 24, 352, 24);

    fOkButton.reset (new TextButton ("okay button"));
    addAndMakeVisible (fOkButton.get());
    fOkButton->setButtonText (TRANS("Okay"));
    fOkButton->addListener (this);

    fOkButton->setBounds (432, 64, 94, 24);


    //[UserPreSize]

   fLookAndFeel = std::make_unique<ATextEditLookAndFeel>();
   fNewNameText->setLookAndFeel(fLookAndFeel.get());

    //[/UserPreSize]

    setSize (600, 120);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

ANameSortFile::~ANameSortFile()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fNewNameText = nullptr;
    fOkButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ANameSortFile::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffe8e8e8));

    {
        int x = 4, y = 20, width = 164, height = 30;
        String text (TRANS("Name your sort file"));
        Colour fillColour = Colour (0xff666666);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (Font::getDefaultSansSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    Justification::centredRight, true);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ANameSortFile::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void ANameSortFile::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == fOkButton.get())
    {
        //[UserButtonCode_fOkButton] -- add your button handler code here..

       this->getTopLevelComponent()->exitModalState(1);

        //[/UserButtonCode_fOkButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ANameSortFile" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="120">
  <BACKGROUND backgroundColour="ffe8e8e8">
    <TEXT pos="4 20 164 30" fill="solid: ff666666" hasStroke="0" text="Name your sort file"
          fontname="Default sans-serif font" fontsize="15.0" kerning="0.0"
          bold="0" italic="0" justification="34"/>
  </BACKGROUND>
  <TEXTEDITOR name="new name text editor" id="b71c5a495ae063c7" memberName="fNewNameText"
              virtualName="" explicitFocusOrder="0" pos="176 24 352 24" textcol="ff666666"
              bkgcol="ffececec" hilitecol="bfecec65" outlinecol="ff222222"
              initialText="" multiline="0" retKeyStartsLine="0" readonly="0"
              scrollbars="0" caret="1" popupmenu="0"/>
  <TEXTBUTTON name="okay button" id="8d13996f0622acb7" memberName="fOkButton"
              virtualName="" explicitFocusOrder="0" pos="432 64 94 24" buttonText="Okay"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

