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

#include <thread>

#include "AppLookAndFeel.h"
#include "Settings.h"

//[/Headers]

#include "SettingsView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SettingsView::SettingsView ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fUsername.reset (new TextEditor ("username text editor"));
    addAndMakeVisible (fUsername.get());
    fUsername->setMultiLine (false);
    fUsername->setReturnKeyStartsNewLine (false);
    fUsername->setReadOnly (false);
    fUsername->setScrollbarsShown (false);
    fUsername->setCaretVisible (false);
    fUsername->setPopupMenuEnabled (false);
    fUsername->setColour (TextEditor::backgroundColourId, Colour (0x00495358));
    fUsername->setColour (TextEditor::outlineColourId, Colour (0xffc8c8c8));
    fUsername->setText (String());

    fServer.reset (new TextEditor ("server text editor"));
    addAndMakeVisible (fServer.get());
    fServer->setMultiLine (false);
    fServer->setReturnKeyStartsNewLine (false);
    fServer->setReadOnly (false);
    fServer->setScrollbarsShown (false);
    fServer->setCaretVisible (false);
    fServer->setPopupMenuEnabled (false);
    fServer->setColour (TextEditor::backgroundColourId, Colour (0x00495358));
    fServer->setColour (TextEditor::outlineColourId, Colour (0xffc8c8c8));
    fServer->setText (String());

    drawable1 = Drawable::createFromImageData (BinaryData::imagesettings_png, BinaryData::imagesettings_pngSize);

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..

   fUsername->addListener(this);
   fServer->addListener(this);

   this->PopulateWithSettings(dontSendNotification);

    //[/Constructor]
}

SettingsView::~SettingsView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fUsername = nullptr;
    fServer = nullptr;
    drawable1 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SettingsView::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0x50111111));

    {
        int x = (getWidth() / 2) - (310 / 2), y = 2, width = 310, height = 102;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (Colours::black);
        jassert (drawable1 != 0);
        if (drawable1 != 0)
            drawable1->drawWithin (g, Rectangle<float> (x, y, width, height),
                                   RectanglePlacement::centred, 1.000f);
    }

    {
        int x = (getWidth() / 2) + -213, y = 180, width = 84, height = 30;
        String text (TRANS("Username:"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (Font::getDefaultSansSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    {
        int x = (getWidth() / 2) + -189, y = 220, width = 52, height = 30;
        String text (TRANS("Server:"));
        Colour fillColour = Colours::white;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.setFont (Font (Font::getDefaultSansSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
        g.drawText (text, x, y, width, height,
                    Justification::centred, true);
    }

    //[UserPaint] Add your own custom painting code here..
   g.setColour(AAppLookAndFeel::kMediumGray);
   g.drawLine(0.0f, getHeight() - 70, getWidth(), getHeight() - 70);
    //[/UserPaint]
}

void SettingsView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fUsername->setBounds ((getWidth() / 2) + -113, 182, 328, 24);
    fServer->setBounds (((getWidth() / 2) + -113) + 0, 222, roundToInt (328 * 1.0000f), roundToInt (24 * 1.0000f));
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SettingsView::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
   this->setSize(this->getParentWidth(), this->getParentHeight());
    //[/UserCode_parentSizeChanged]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...


void SettingsView::textEditorFocusLost(TextEditor& textEditor)
{
   if (fUsername.get() == &textEditor)
   {
      ASettings::Instance().SetUsername(fUsername->getText().toStdString());
   }
   else if (fServer.get() == &textEditor)
   {
      ASettings::Instance().SetServer(fServer->getText().toStdString());
   }
}


// MARK: -

void SettingsView::LeavingView()
{
   ASettings::Instance().SetUsername(fUsername->getText().toStdString());
   ASettings::Instance().SetServer(fServer->getText().toStdString());
}

void SettingsView::PopulateWithSettings(NotificationType notify)
{
   const auto& settings = ASettings::Instance();

   fUsername->setText(settings.Username(), notify);
   fServer->setText(settings.Server(), notify);
}


//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SettingsView" componentName=""
                 parentClasses="public ContentView, public TextEditor::Listener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="50111111">
    <IMAGE pos="0Cc 2 310 102" resource="BinaryData::imagesettings_png"
           opacity="1.0" mode="1"/>
    <TEXT pos="-213C 180 84 30" fill="solid: ffffffff" hasStroke="0" text="Username:"
          fontname="Default sans-serif font" fontsize="15.0" kerning="0.0"
          bold="0" italic="0" justification="36"/>
    <TEXT pos="-189C 220 52 30" fill="solid: ffffffff" hasStroke="0" text="Server:"
          fontname="Default sans-serif font" fontsize="15.0" kerning="0.0"
          bold="0" italic="0" justification="36"/>
  </BACKGROUND>
  <TEXTEDITOR name="username text editor" id="9144e7cf0a01cad7" memberName="fUsername"
              virtualName="" explicitFocusOrder="0" pos="-113C 182 328 24"
              bkgcol="495358" outlinecol="ffc8c8c8" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="0" caret="0" popupmenu="0"/>
  <TEXTEDITOR name="server text editor" id="623493249758a0e4" memberName="fServer"
              virtualName="" explicitFocusOrder="0" pos="0 222 100% 100%" posRelativeX="9144e7cf0a01cad7"
              posRelativeW="9144e7cf0a01cad7" posRelativeH="9144e7cf0a01cad7"
              bkgcol="495358" outlinecol="ffc8c8c8" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="0" caret="0" popupmenu="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

