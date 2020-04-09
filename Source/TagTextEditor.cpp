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

#include <algorithm>
#include <iostream>

#include "TagButton.h"

//[/Headers]

#include "TagTextEditor.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
namespace
{
   constexpr int sMakeItemId(int classId) noexcept { return classId + 1; }
   constexpr int sGetClassId(int itemId) noexcept  { return itemId - 1; }
   constexpr const char* const kTokenizeCharSet = " \t\r\f\n,";

   constexpr float kCornerSize = 10.0f;
}
//[/MiscUserDefs]

//==============================================================================
ATagTextEditor::ATagTextEditor ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fTextEditor.reset (new TextEditor (" text editor"));
    addAndMakeVisible (fTextEditor.get());
    fTextEditor->setMultiLine (false);
    fTextEditor->setReturnKeyStartsNewLine (false);
    fTextEditor->setReadOnly (false);
    fTextEditor->setScrollbarsShown (false);
    fTextEditor->setCaretVisible (true);
    fTextEditor->setPopupMenuEnabled (false);
    fTextEditor->setColour (TextEditor::textColourId, Colour (0xff666666));
    fTextEditor->setColour (TextEditor::backgroundColourId, Colours::white);
    fTextEditor->setColour (TextEditor::highlightColourId, Colour (0xffe0ec65));
    fTextEditor->setColour (TextEditor::outlineColourId, Colour (0x00c8c8c8));
    fTextEditor->setColour (CaretComponent::caretColourId, Colour (0xff666666));
    fTextEditor->setText (String());

    fTextEditor->setBounds (0, 0, 300, 30);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (300, 30);


    //[Constructor] You can add your own custom stuff here..

   fTagsSource = ATagsSource();

   fTextEditor->addListener(this);
   fTextEditor->setBorder(BorderSize<int>(0));
   // Hide focus outline
   fTextEditor->setColour(TextEditor::focusedOutlineColourId, Colour(0x00000000));
   fTextEditor->setTabKeyUsedAsCharacter(true);

   fCompletionPopup = std::make_unique<PopupMenu>();

    //[/Constructor]
}

ATagTextEditor::~ATagTextEditor()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fTextEditor = nullptr;


    //[Destructor]. You can add your own custom destruction code here..

   this->ClearTokens();
   fCompletionPopup = nullptr;

    //[/Destructor]
}

//==============================================================================
void ATagTextEditor::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    //[UserPaint] Add your own custom painting code here..

   g.setColour(Colour(0xffffffff));// !!! Find nice way to get this color
   const auto bounds = this->getLocalBounds().toFloat();
   g.fillRoundedRectangle(bounds, kCornerSize);
   g.setColour(Colour(0xffc8c8c8));
   g.drawRoundedRectangle(bounds, kCornerSize, 1.0f);

    //[/UserPaint]
}

void ATagTextEditor::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..

   this->Layout();

    //[/UserResized]
}

void ATagTextEditor::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
   this->Layout();
    //[/UserCode_parentSizeChanged]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...


void ATagTextEditor::Clear()
{
   this->ClearTokens();
   fTextEditor->clear();
   fTextEditor->repaint();
   fTags.clear();
   
   this->Layout();
   this->sendChangeMessage();
}


void ATagTextEditor::ClearTokens()
{
   for (const auto& button : fTagButtons)
   {
      this->RemoveToken(button);
   }
   jassert(fTagButtons.empty());
}


void ATagTextEditor::CompleteTagName(String& tagName) const
{
   fTagsSource.Complete(tagName.toStdString(), [&tagName](const auto& tag)
    {
       tagName = tag.Name();
       // We only need the first match
       return false;
    });
}


const Tags& ATagTextEditor::CurrentTags() const
{
   return fTags;
}


void ATagTextEditor::Layout()
{
   auto fb = FlexBox();
   fb.flexWrap = FlexBox::Wrap::noWrap;
   fb.justifyContent = FlexBox::JustifyContent::flexStart;
   fb.alignContent = FlexBox::AlignContent::stretch;

   for (auto& tagButton : fTagButtons)
   {
      fb.items.add(FlexItem(*tagButton)
                   .withMargin(3.0f)
                   .withMinWidth(tagButton->getWidth())
                   .withMaxWidth(tagButton->getWidth())
                   .withMinHeight(tagButton->getHeight())
                   .withMaxHeight(tagButton->getHeight())
                   );
   }
   fb.items.add(FlexItem(*fTextEditor)
                .withMargin(3.0f)
                .withMinWidth(100.0f)
                .withMaxWidth(this->getWidth() - 6.0f)
                .withMinHeight(this->getHeight() - 6.0f)
                .withMaxHeight(this->getHeight() - 6.0f)
                );

   fb.performLayout(this->getLocalBounds().toFloat());
}


void ATagTextEditor::PopulateCompletionPopup(String& text)
{
   PopupMenu::dismissAllActiveMenus();
   fCompletionPopup->clear();
   
   fTagsSource.Complete(text.toStdString(), [this](const auto& tag)
    {
       fCompletionPopup->addItem(sMakeItemId(tag.ClassId()), String(tag.Name()));
       return true;
    });
}

void ATagTextEditor::RemoveToken(ATagButton* button)
{
   if (nullptr == button)
   {
      return;
   }

   auto pos = std::find(fTagButtons.begin(), fTagButtons.end(), button);
   if (fTagButtons.end() != pos)
   {
      fTagButtons.erase(pos);

      std::string name = button->getButtonText().toStdString();
      fTags.erase(std::remove_if(fTags.begin(), fTags.end(),
       [name](const auto& tag)
       {
          return name == tag.Name();
       }),
       fTags.end());

      this->removeChildComponent(button);
      delete button;
   }
   else
   {
      std::cerr << "Trying to remove a tag button which isn't known to ATagTextEditor: "
                << button->getButtonText()
                << "\n";
   }
}

void ATagTextEditor::SetSortFile(const ASortFile& sortFile)
{
   fTagsSource = ATagsSource(sortFile);
}

void ATagTextEditor::ShowCompletionPopup()
{
   auto options = PopupMenu::Options().withTargetComponent(fTextEditor.get());
   
   fCompletionPopup->showMenuAsync(options, [this](int itemId)
    {
       if (0 != itemId) // I think this is sent when the menu is displayed and hidden
       {
          if (this->TokenizeTagMatching(sGetClassId(itemId)))
          {
             fTextEditor->clear();
          }
          else
          {
             fTextEditor->getLookAndFeel().playAlertSound();
          }
       }
    });
}

bool ATagTextEditor::TokenizeTagMatching(int classId)
{
   auto matches = fTagsSource.Matches(classId);
   if (!matches.empty())
   {
      this->TokenizeTag(matches.front());
      return true;
   }
   return false;
}


bool ATagTextEditor::TokenizeTagMatching(const String& name)
{
   auto matches = fTagsSource.Matches(name.toStdString());
   if (!matches.empty())
   {
      this->TokenizeTag(matches.front());
      return true;
   }
   return false;
}


void ATagTextEditor::TokenizeTag(const ATag& tag)
{
   if (fTags.cend() == std::find(fTags.cbegin(), fTags.cend(), tag))
   {
      fTags.push_back(tag);

      auto tagButton = new ATagButton(tag.Name());
      tagButton->SetShowClose(true);
      tagButton->OnCloseClick = [tagButton, this]()
      {
         this->RemoveToken(tagButton);
         this->Layout();
         this->sendChangeMessage();
      };

      auto button = fTagButtons.emplace_back(tagButton);
      button->addListener(this);
      this->addAndMakeVisible(button);

      this->Layout();
      this->sendChangeMessage();
   }
}


bool ATagTextEditor::TryToComplete(String tagName) // yes, by value
{
   const bool doComplete = (tagName.getLastCharacter() == KeyPress::tabKey);
   
   tagName = tagName.trimCharactersAtEnd(kTokenizeCharSet);
   
   if (doComplete)
   {
      this->CompleteTagName(tagName);
   }
   
   // ???: Complete to ATag then just tokenize that?
   return this->TokenizeTagMatching(tagName);
}


void ATagTextEditor::buttonClicked(Button* button)
{
   // Do nothing. If X is clicked in tag button, it is handled by its OnCloseClick.
}


void ATagTextEditor::focusGained(FocusChangeType)
{
   auto text = fTextEditor->getText();
   this->PopulateCompletionPopup(text);
   this->ShowCompletionPopup();
}


void ATagTextEditor::textEditorTextChanged(TextEditor& te)
{
   auto text = te.getText();
   if (text.isNotEmpty() && (1 != text.length())
     && text.getLastCharacters(1).containsAnyOf(kTokenizeCharSet))
   {
      if (this->TryToComplete(text))
      {
         te.clear();
      }
   }
   else
   {
      text = text.trimCharactersAtEnd(kTokenizeCharSet);
      
      this->PopulateCompletionPopup(text);
      this->ShowCompletionPopup();
      
      // Don't leave the tokenizing chars in the buffer
      te.setText(text);
   }
}


void ATagTextEditor::textEditorReturnKeyPressed(TextEditor& te)
{
   auto text = te.getText().trimCharactersAtEnd(kTokenizeCharSet);
   if (this->TryToComplete(text))
   {
      te.clear();
   }
}


//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="ATagTextEditor" componentName=""
                 parentClasses="public Component, public TextEditor::Listener, public Button::Listener, public ChangeBroadcaster"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="300"
                 initialHeight="30">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="ebebeb"/>
  <TEXTEDITOR name=" text editor" id="b9a228722d24e189" memberName="fTextEditor"
              virtualName="" explicitFocusOrder="0" pos="0 0 300 30" textcol="ff666666"
              bkgcol="ffffffff" hilitecol="ffe0ec65" outlinecol="c8c8c8" caretcol="ff666666"
              initialText="" multiline="0" retKeyStartsLine="0" readonly="0"
              scrollbars="0" caret="1" popupmenu="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

