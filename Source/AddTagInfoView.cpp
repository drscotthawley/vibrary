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

#include "AddTagInfoView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AAddTagInfoView::AAddTagInfoView ()
{
    //[Constructor_pre] You can add your own custom stuff here..
   // TODO: !!!: The slider may not work as a progress bar. There is a progress
   //             bar class but it doesn't seem to have Projucer support.
    //[/Constructor_pre]

    fTagStrengthView.reset (new Slider ("tag strength text"));
    addAndMakeVisible (fTagStrengthView.get());
    fTagStrengthView->setRange (0, 10, 0);
    fTagStrengthView->setSliderStyle (Slider::LinearBar);
    fTagStrengthView->setTextBoxStyle (Slider::NoTextBox, true, 100, 20);
    fTagStrengthView->setColour (Slider::backgroundColourId, Colour (0xff666666));
    fTagStrengthView->setColour (Slider::trackColourId, Colour (0xff69c27a));
    fTagStrengthView->setColour (Slider::textBoxTextColourId, Colour (0xff666666));
    fTagStrengthView->setColour (Slider::textBoxBackgroundColourId, Colour (0xff263238));

    fTagStrengthView->setBounds (8, 39, 255, 33);

    fSomeFilesHelpLabel.reset (new Label ("some files help label",
                                          TRANS("Increase your tag strength by adding more examples to this tag.")));
    addAndMakeVisible (fSomeFilesHelpLabel.get());
    fSomeFilesHelpLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 14.00f, Font::plain).withTypefaceStyle ("Regular"));
    fSomeFilesHelpLabel->setJustificationType (Justification::centredLeft);
    fSomeFilesHelpLabel->setEditable (false, false, false);
    fSomeFilesHelpLabel->setColour (Label::textColourId, Colour (0xff666666));
    fSomeFilesHelpLabel->setColour (TextEditor::textColourId, Colours::black);
    fSomeFilesHelpLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fSomeFilesHelpLabel->setBounds (12, 70, 255, 48);

    fStrengthLabel.reset (new Label ("strength label",
                                     TRANS("Tag strength:\n")));
    addAndMakeVisible (fStrengthLabel.get());
    fStrengthLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    fStrengthLabel->setJustificationType (Justification::centredLeft);
    fStrengthLabel->setEditable (false, false, false);
    fStrengthLabel->setColour (Label::textColourId, Colour (0xff666666));
    fStrengthLabel->setColour (TextEditor::textColourId, Colours::black);
    fStrengthLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fStrengthLabel->setBounds (10, 11, 104, 24);

    fStrengthPercentLabel.reset (new Label ("strength percent label",
                                            TRANS("0%")));
    addAndMakeVisible (fStrengthPercentLabel.get());
    fStrengthPercentLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Bold"));
    fStrengthPercentLabel->setJustificationType (Justification::centredLeft);
    fStrengthPercentLabel->setEditable (false, false, false);
    fStrengthPercentLabel->setColour (Label::textColourId, Colour (0xff666666));
    fStrengthPercentLabel->setColour (TextEditor::textColourId, Colours::black);
    fStrengthPercentLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    //[/UserPreSize]

    setSize (298, 128);


    //[Constructor] You can add your own custom stuff here..
   fTagStrengthView->setEnabled(false);
    //[/Constructor]
}

AAddTagInfoView::~AAddTagInfoView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fTagStrengthView = nullptr;
    fSomeFilesHelpLabel = nullptr;
    fStrengthLabel = nullptr;
    fStrengthPercentLabel = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AAddTagInfoView::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffebebeb));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AAddTagInfoView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fStrengthPercentLabel->setBounds (10 + 104, 11, 104, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void AAddTagInfoView::handleCommandMessage(int commandId)
{
   jassert(commandId > 0);
   if ((AddTags::kUpdateStrength & (uint32_t)commandId) != 0U)
   {
      this->SetStrength((uint32_t)commandId ^ AddTags::kUpdateStrength);
   }
}


void AAddTagInfoView::SetStrength(size_t exampleCount)
{
   // TODO: Something smarter
   String percent;
   percent << (exampleCount * 100 / 20) << "%";
   fStrengthPercentLabel->setText(percent, dontSendNotification);

   fTagStrengthView->setValue(exampleCount);
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AAddTagInfoView" componentName=""
                 parentClasses="public Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="298" initialHeight="128">
  <BACKGROUND backgroundColour="ffebebeb"/>
  <SLIDER name="tag strength text" id="74909218c3d8b8bf" memberName="fTagStrengthView"
          virtualName="" explicitFocusOrder="0" pos="8 39 255 33" bkgcol="ff666666"
          trackcol="ff69c27a" textboxtext="ff666666" textboxbkgd="ff263238"
          min="0.0" max="10.0" int="0.0" style="LinearBar" textBoxPos="NoTextBox"
          textBoxEditable="0" textBoxWidth="100" textBoxHeight="20" skewFactor="1.0"
          needsCallback="0"/>
  <LABEL name="some files help label" id="37e198792c1a88ce" memberName="fSomeFilesHelpLabel"
         virtualName="" explicitFocusOrder="0" pos="12 70 255 48" textCol="ff666666"
         edTextCol="ff000000" edBkgCol="0" labelText="Increase your tag strength by adding more examples to this tag."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default sans-serif font" fontsize="14.0" kerning="0.0"
         bold="0" italic="0" justification="33"/>
  <LABEL name="strength label" id="1df7a4b311cdb287" memberName="fStrengthLabel"
         virtualName="" explicitFocusOrder="0" pos="10 11 104 24" textCol="ff666666"
         edTextCol="ff000000" edBkgCol="0" labelText="Tag strength:&#10;"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default sans-serif font" fontsize="16.0" kerning="0.0"
         bold="0" italic="0" justification="33"/>
  <LABEL name="strength percent label" id="e3e97ffef223780" memberName="fStrengthPercentLabel"
         virtualName="" explicitFocusOrder="0" pos="0R 11 104 24" posRelativeX="1df7a4b311cdb287"
         textCol="ff666666" edTextCol="ff000000" edBkgCol="0" labelText="0%"
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default sans-serif font" fontsize="16.0" kerning="0.0"
         bold="1" italic="0" justification="33" typefaceStyle="Bold"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

