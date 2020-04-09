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

#include "ProgressOverlay.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AProgressOverlay::AProgressOverlay ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fAddingLabel.reset (new Label ("adding label",
                                   TRANS("Adding ")));
    addAndMakeVisible (fAddingLabel.get());
    fAddingLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 15.00f, Font::plain).withTypefaceStyle ("Regular"));
    fAddingLabel->setJustificationType (Justification::centredLeft);
    fAddingLabel->setEditable (false, false, false);
    fAddingLabel->setColour (TextEditor::textColourId, Colours::black);
    fAddingLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fCountLabel.reset (new Label ("count label",
                                  TRANS("999")));
    addAndMakeVisible (fCountLabel.get());
    fCountLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 24.00f, Font::plain).withTypefaceStyle ("Bold"));
    fCountLabel->setJustificationType (Justification::centredRight);
    fCountLabel->setEditable (false, false, false);
    fCountLabel->setColour (TextEditor::textColourId, Colours::black);
    fCountLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fFilesLabel.reset (new Label ("files label",
                                  TRANS("files")));
    addAndMakeVisible (fFilesLabel.get());
    fFilesLabel->setFont (Font (15.00f, Font::plain).withTypefaceStyle ("Regular"));
    fFilesLabel->setJustificationType (Justification::centredLeft);
    fFilesLabel->setEditable (false, false, false);
    fFilesLabel->setColour (TextEditor::textColourId, Colours::black);
    fFilesLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fSpectraImage.reset (new ImageComponent());
    addAndMakeVisible (fSpectraImage.get());
    fSpectraImage->setName ("spectra image");


    //[UserPreSize]
    //[/UserPreSize]

    setSize (683, 510);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

AProgressOverlay::~AProgressOverlay()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fAddingLabel = nullptr;
    fCountLabel = nullptr;
    fFilesLabel = nullptr;
    fSpectraImage = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AProgressOverlay::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
   AOverlayView::paint(g);
    //[/UserPrePaint]

    {
        float x = static_cast<float> ((getWidth() / 2) - (300 / 2)), y = static_cast<float> ((getHeight() / 2) - (69 / 2)), width = 300.0f, height = 69.0f;
        Colour fillColour = Colour (0xff222222);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 20.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AProgressOverlay::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fAddingLabel->setBounds ((getWidth() / 2) + -104 - (56 / 2), (getHeight() / 2) - (15 / 2), 56, 15);
    fCountLabel->setBounds (((getWidth() / 2) + -104 - (56 / 2)) + 56 - -3, ((getHeight() / 2) - (15 / 2)) + 15 / 2 - ((roundToInt (15 * 1.0000f)) / 2), 45, roundToInt (15 * 1.0000f));
    fFilesLabel->setBounds ((((getWidth() / 2) + -104 - (56 / 2)) + 56 - -3) + 45, ((getHeight() / 2) - (15 / 2)) + 0, 40, roundToInt (15 * 1.0000f));
    fSpectraImage->setBounds (((((getWidth() / 2) + -104 - (56 / 2)) + 56 - -3) + 45) + 40 - -51, (getHeight() / 2) - (54 / 2), 65, 54);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void AProgressOverlay::handleAsyncUpdate()
{
   if (fExit)
   {
      fExit = false;
      this->handleCommandMessage(Overlay::kExit);
   }
   else
   {
      if (fCountLabel->getText() != String(fFileCount))
      {
         fCountLabel->setText(String(fFileCount), dontSendNotification);
      }
      this->DisplayNextImage();
   }
}

void AProgressOverlay::AsyncExit()
{
   fExit = true;
   this->triggerAsyncUpdate();
}


void AProgressOverlay::DisplayNextImage()
{
   ScopedLock guard(fImageFileLock);

   fSpectraImage->setImage(std::move(fNextImage));
}


void AProgressOverlay::SetFileCount(int count)
{
   if (count != fFileCount)
   {
      fFileCount = count;
      this->triggerAsyncUpdate();
   }
}


void AProgressOverlay::SetNextImage(const Image& image)
{
   if (image.isValid())
   {
      ScopedLock guard(fImageFileLock);

      fNextImage = image;
      this->triggerAsyncUpdate();
   }
}


void AProgressOverlay::SetNextImageFile(const File& imageFile)
{
   if (imageFile.existsAsFile())
   {
      ScopedLock guard(fImageFileLock);

      fNextImage = ImageCache::getFromFile(imageFile);
      this->triggerAsyncUpdate();
   }
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AProgressOverlay" componentName=""
                 parentClasses="public AOverlayView, public AsyncUpdater" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="683" initialHeight="510">
  <BACKGROUND backgroundColour="222222">
    <ROUNDRECT pos="0Cc 0Cc 300 69" cornerSize="20.0" fill="solid: ff222222"
               hasStroke="0"/>
  </BACKGROUND>
  <LABEL name="adding label" id="b2c4003ae207c685" memberName="fAddingLabel"
         virtualName="" explicitFocusOrder="0" pos="-104Cc 0.5Cc 56 15"
         edTextCol="ff000000" edBkgCol="0" labelText="Adding " editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="count label" id="5fe0fcf6e6b9fb69" memberName="fCountLabel"
         virtualName="" explicitFocusOrder="0" pos="-3R -0.5Cc 45 100%"
         posRelativeX="b2c4003ae207c685" posRelativeY="b2c4003ae207c685"
         posRelativeH="b2c4003ae207c685" edTextCol="ff000000" edBkgCol="0"
         labelText="999" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default sans-serif font" fontsize="24.0"
         kerning="0.0" bold="1" italic="0" justification="34" typefaceStyle="Bold"/>
  <LABEL name="files label" id="e1d5ade517c3726d" memberName="fFilesLabel"
         virtualName="" explicitFocusOrder="0" pos="0R 0 40 100%" posRelativeX="5fe0fcf6e6b9fb69"
         posRelativeY="b2c4003ae207c685" posRelativeH="b2c4003ae207c685"
         edTextCol="ff000000" edBkgCol="0" labelText="files" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="spectra image" id="de9c1c53a205d524" memberName="fSpectraImage"
                    virtualName="" explicitFocusOrder="0" pos="-51R 0Cc 65 54" posRelativeX="e1d5ade517c3726d"
                    class="ImageComponent" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

