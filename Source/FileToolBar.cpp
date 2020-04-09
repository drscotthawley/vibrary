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
#include "SortFileComboBox.h"
#include "SortFilesSource.h"
#include "TagTextEditor.h"

//[/Headers]

#include "FileToolBar.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AFileToolBar::AFileToolBar ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fTagTokenizer.reset (new ATagTextEditor());
    addAndMakeVisible (fTagTokenizer.get());
    fTagTokenizer->setExplicitFocusOrder (1);
    fTagTokenizer->setName ("tag tokenzier");

    fSortFileComboBox.reset (new SortFileComboBox (SortFileComboBox::InitialText::kChoose));
    addAndMakeVisible (fSortFileComboBox.get());
    fSortFileComboBox->setName ("sort file combo box");

    fSortFileComboBox->setBounds (8, 9, 169, 30);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (894, 48);


    //[Constructor] You can add your own custom stuff here..

   fSortFileComboBox->addListener(this);

   fTagTokenizer->addChangeListener(this);

    //[/Constructor]
}

AFileToolBar::~AFileToolBar()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fTagTokenizer = nullptr;
    fSortFileComboBox = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AFileToolBar::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xffebebeb));

    {
        int x = 0, y = 0, width = 894, height = 48;
        Colour fillColour = Colour (0xffebebeb);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRect (x, y, width, height);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AFileToolBar::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fTagTokenizer->setBounds (185, 8, getWidth() - 297, 30);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void AFileToolBar::changeListenerCallback(ChangeBroadcaster* source)
{
   if (fTagTokenizer.get() == source)
   {
      this->sendChangeMessage();
   }
}

void AFileToolBar::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
   if (comboBoxThatHasChanged == fSortFileComboBox.get())
   {
      auto selectedId = fSortFileComboBox->getSelectedId();
      if (fSortFileComboBox->CreateItemId() == selectedId)
      {
         // TODO: Launch create sort file workflow
      }
      else
      {
         fTagTokenizer->Clear();

         fSortFile = fSortFileComboBox->SelectedSortFile();
         fTagTokenizer->SetSortFile(fSortFile);

         this->sendChangeMessage();
      }
   }
}

void AFileToolBar::handleCommandMessage(int commandId)
{
   switch (commandId)
   {
      case Training::kSortFileAdded:
      fSortFileComboBox->Populate(true);
      break;
   }
}


const ASortFile& AFileToolBar::SelectedSortFile() const noexcept
{
   return fSortFile;
}



Tags AFileToolBar::TagsToInclude() const
{
   return fTagTokenizer->CurrentTags();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AFileToolBar" componentName=""
                 parentClasses="public Component, public ChangeListener, public ChangeBroadcaster, public ComboBox::Listener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="894"
                 initialHeight="48">
  <BACKGROUND backgroundColour="ffebebeb">
    <RECT pos="0 0 894 48" fill="solid: ffebebeb" hasStroke="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="tag tokenzier" id="a7eaf9b05ac338d8" memberName="fTagTokenizer"
                    virtualName="" explicitFocusOrder="1" pos="185 8 297M 30" class="ATagTextEditor"
                    params=""/>
  <GENERICCOMPONENT name="sort file combo box" id="c2fc59734a5c17ea" memberName="fSortFileComboBox"
                    virtualName="" explicitFocusOrder="0" pos="8 9 169 30" class="SortFileComboBox"
                    params="SortFileComboBox::InitialText::kChoose"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

