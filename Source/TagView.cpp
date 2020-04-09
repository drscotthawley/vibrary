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

#include <algorithm>

#include "ButtonLookAndFeel.h"
#include "CommandMessages.h"
#include "SortFile.h"
#include "TagButton.h"
#include "TagsSource.h"

//[/Headers]

#include "TagView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...

//[/MiscUserDefs]

//==============================================================================
ATagView::ATagView ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fAddTagButton.reset (new ATagButton (ATag("Add Tag")));
    addAndMakeVisible (fAddTagButton.get());
    fAddTagButton->setName ("add tag button");

    fAddTagButton->setBounds (16, 16, 109, 24);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..

   fAddTagButton->addListener(this);

    //[/Constructor]
}

ATagView::~ATagView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
   this->ClearButtons();
    //[/Destructor_pre]

    fAddTagButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void ATagView::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void ATagView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
   this->LayoutButtons();
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void ATagView::AddTag(const ATag& tag)
{
   fTags.push_back(tag);
   this->AddTagButton(tag);
   this->LayoutButtons();
}


void ATagView::AddTagButton(const ATag& tag)
{
   auto tagButton = new ATagButton(tag);
   tagButton->SetShowClose(true);
   tagButton->OnCloseClick =
    [tagButton, this]()
    {
       this->RemoveTag(tagButton);
    };

   auto button = fButtons.emplace_back(tagButton);
   button->addListener(this);
   button->setEnabled(true);

   this->addAndMakeVisible(button);
}


void ATagView::ClearButtons()
{
   for (auto* b : fButtons)
   {
      this->removeChildComponent(b);
      delete b;
   }
   fButtons.clear();
}


void ATagView::LayoutButtons()
{
   constexpr float kMargin = 5.0f;

   FlexBox fb;
   fb.flexWrap = FlexBox::Wrap::wrap;
   fb.justifyContent = FlexBox::JustifyContent::flexStart;
   fb.alignContent = FlexBox::AlignContent::flexStart;
   for (auto* b : fButtons)
   {
      fb.items.add(FlexItem(*b)
                   .withMargin(kMargin)
                   .withMinWidth(b->getWidth())
                   .withMinHeight(b->getHeight())
                   );
   }
   fb.items.add(FlexItem(*fAddTagButton)
                        .withMargin(kMargin)
                        .withMinWidth(fAddTagButton->getWidth())
                        .withMinHeight(fAddTagButton->getHeight())
                        );
   fb.performLayout(this->getLocalBounds().toFloat());
}


void ATagView::RemoveTag(const ATagButton* button)
{
   auto tagIt = std::find(fTags.begin(), fTags.end(), button->Tag());
   if (fTags.end() != tagIt)
   {
      fTags.erase(tagIt);
   }

   auto buttonIt = std::find_if(fButtons.begin(), fButtons.end(),
    [button](const auto& b)
    {
       return (void*)button == (void*)b;
    });
   if (fButtons.end() != buttonIt)
   {
      fButtons.erase(buttonIt);
   }

   this->removeChildComponent((Button*)button);
   delete button;

   this->LayoutButtons();
}


void ATagView::SetTagFunction(TagFunction function)
{
   fTagFunction = function;
}


void ATagView::SetTags(const Tags& tags)
{
   fTags = tags;
   std::sort(std::begin(fTags), std::end(fTags));

   this->ClearButtons();
   fButtons.reserve(tags.size());

   for (const auto& tag : fTags)
   {
      this->AddTagButton(tag);
   }
   this->LayoutButtons();

   this->repaint();
}

void ATagView::SetTagsFrom(const ASortFile& sortFile)
{
  this->SetTags(ATagsSource(sortFile).All());
}

void ATagView::buttonClicked(Button* buttonThatWasClicked)
{
   jassert(fTags.size() == fButtons.size());

   if (buttonThatWasClicked == fAddTagButton.get())
   {
      if (nullptr != this->getParentComponent())
      {
         this->getParentComponent()->postCommandMessage(AddTags::kAddTag);
      }
   }
   else // it's a tag button
   {
      auto tagIt = fTags.begin();
      auto buttonIt = fButtons.cbegin();
      auto endTagButtons = fButtons.cend();

      while (endTagButtons != buttonIt && buttonThatWasClicked != *buttonIt++)
      {
         ++tagIt;
      }

      if (fTags.end() != tagIt)
      {
         fTagFunction(*tagIt);
      }
   }
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ATagView" componentName=""
                 parentClasses="public Component, public Button::Listener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <GENERICCOMPONENT name="add tag button" id="39bbd8d81af49498" memberName="fAddTagButton"
                    virtualName="" explicitFocusOrder="0" pos="16 16 109 24" class="ATagButton"
                    params="ATag(&quot;Add Tag&quot;)"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

