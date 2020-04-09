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

#include "SimpleFileTable.h"
#include "Tag.h"
#include "Tagger.h"
#include "TextEditLookAndFeel.h"
#include "Utils.h"

//[/Headers]

#include "AddTagsView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
AAddTagsView::AAddTagsView ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fFileTable.reset (new ASimpleFileTable());
    addAndMakeVisible (fFileTable.get());
    fFileTable->setExplicitFocusOrder (3);
    fFileTable->setName ("file table");

    fEmptyFileView.reset (new AEmptyAddTagFileView());
    addAndMakeVisible (fEmptyFileView.get());
    fEmptyFileView->setExplicitFocusOrder (3);
    fEmptyFileView->setName ("empty file view");

    fAlsoDragLabel.reset (new Label ("also drag label",
                                     TRANS("You can also drag and drop.")));
    addAndMakeVisible (fAlsoDragLabel.get());
    fAlsoDragLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 13.00f, Font::italic));
    fAlsoDragLabel->setJustificationType (Justification::centredLeft);
    fAlsoDragLabel->setEditable (false, false, false);
    fAlsoDragLabel->setColour (Label::textColourId, Colour (0xff666666));
    fAlsoDragLabel->setColour (TextEditor::textColourId, Colours::black);
    fAlsoDragLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fAlsoDragLabel->setBounds (24, 119, 209, 20);

    fTagNameLabel.reset (new Label ("tag name label",
                                    TRANS("Tag name:")));
    addAndMakeVisible (fTagNameLabel.get());
    fTagNameLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 16.00f, Font::plain).withTypefaceStyle ("Regular"));
    fTagNameLabel->setJustificationType (Justification::centredLeft);
    fTagNameLabel->setEditable (false, false, false);
    fTagNameLabel->setColour (Label::textColourId, Colour (0xff666666));
    fTagNameLabel->setColour (TextEditor::textColourId, Colours::black);
    fTagNameLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fTagNameLabel->setBounds (20, 60, 93, 20);

    fTagInfoView.reset (new AAddTagInfoView());
    addAndMakeVisible (fTagInfoView.get());
    fTagInfoView->setName ("tag info view");

    fTagInfoView->setBounds (14, 283, 298, 128);

    fTagsLabel.reset (new Label ("tags label",
                                 TRANS("Tags")));
    addAndMakeVisible (fTagsLabel.get());
    fTagsLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 17.00f, Font::plain).withTypefaceStyle ("Bold"));
    fTagsLabel->setJustificationType (Justification::centredLeft);
    fTagsLabel->setEditable (false, false, false);
    fTagsLabel->setColour (Label::textColourId, Colour (0xff666666));
    fTagsLabel->setColour (TextEditor::textColourId, Colours::black);
    fTagsLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fTagsLabel->setBounds (21, 14, 213, 20);

    fTagNameTextBox.reset (new TextEditor ("tag name text box"));
    addAndMakeVisible (fTagNameTextBox.get());
    fTagNameTextBox->setExplicitFocusOrder (1);
    fTagNameTextBox->setMultiLine (false);
    fTagNameTextBox->setReturnKeyStartsNewLine (false);
    fTagNameTextBox->setReadOnly (false);
    fTagNameTextBox->setScrollbarsShown (false);
    fTagNameTextBox->setCaretVisible (true);
    fTagNameTextBox->setPopupMenuEnabled (false);
    fTagNameTextBox->setColour (TextEditor::textColourId, Colour (0xff666666));
    fTagNameTextBox->setColour (TextEditor::backgroundColourId, Colour (0xffececec));
    fTagNameTextBox->setColour (TextEditor::outlineColourId, Colour (0xffc8c8c8));
    fTagNameTextBox->setColour (CaretComponent::caretColourId, Colour (0xffececec));
    fTagNameTextBox->setText (String());

    fDisableTagButton.reset (new ToggleButton ("disable tag button"));
    addAndMakeVisible (fDisableTagButton.get());
    fDisableTagButton->setExplicitFocusOrder (5);
    fDisableTagButton->setButtonText (TRANS("Disable this tag"));
    fDisableTagButton->addListener (this);
    fDisableTagButton->setColour (ToggleButton::textColourId, Colour (0xff666666));

    fDisableTagButton->setBounds (20, 224, 150, 27);

    fSaveButton.reset (new TextButton ("save button"));
    addAndMakeVisible (fSaveButton.get());
    fSaveButton->setExplicitFocusOrder (4);
    fSaveButton->setButtonText (TRANS("Save Changes"));
    fSaveButton->addListener (this);
    fSaveButton->setColour (TextButton::textColourOffId, Colour (0xffdee1df));

    fAddButton.reset (new TextButton ("add button"));
    addAndMakeVisible (fAddButton.get());
    fAddButton->setExplicitFocusOrder (2);
    fAddButton->setButtonText (TRANS("+"));
    fAddButton->addListener (this);
    fAddButton->setColour (TextButton::textColourOffId, Colour (0xffebebeb));

    fAddButton->setBounds (236, 101, 38, 38);

    fDeleteButton.reset (new TextButton ("delete button"));
    addAndMakeVisible (fDeleteButton.get());
    fDeleteButton->setButtonText (TRANS("Delete this tag"));
    fDeleteButton->addListener (this);
    fDeleteButton->setColour (TextButton::buttonColourId, Colour (0x00a45c94));
    fDeleteButton->setColour (TextButton::buttonOnColourId, Colour (0x00181f22));
    fDeleteButton->setColour (TextButton::textColourOffId, Colour (0xfff6241f));
    fDeleteButton->setColour (TextButton::textColourOnId, Colour (0xfff6241f));

    fDeleteButton->setBounds (171, 224, 112, 27);

    fAddMoreFilesButton.reset (new TextButton ("add more files button"));
    addAndMakeVisible (fAddMoreFilesButton.get());
    fAddMoreFilesButton->setExplicitFocusOrder (4);
    fAddMoreFilesButton->setButtonText (TRANS("Add More Files"));
    fAddMoreFilesButton->addListener (this);
    fAddMoreFilesButton->setColour (TextButton::textColourOffId, Colour (0xffdee1df));

    fAddMoreFilesButton->setBounds (21, 160, 254, 50);

    fAddExamplesText.reset (new TextEditor ("add examples text"));
    addAndMakeVisible (fAddExamplesText.get());
    fAddExamplesText->setMultiLine (false);
    fAddExamplesText->setReturnKeyStartsNewLine (false);
    fAddExamplesText->setReadOnly (true);
    fAddExamplesText->setScrollbarsShown (false);
    fAddExamplesText->setCaretVisible (false);
    fAddExamplesText->setPopupMenuEnabled (false);
    fAddExamplesText->setColour (TextEditor::textColourId, Colour (0xff666666));
    fAddExamplesText->setColour (TextEditor::backgroundColourId, Colour (0x00495358));
    fAddExamplesText->setText (TRANS("Add examples for this tag."));

    fAddExamplesText->setBounds (24, 101, 209, 20);

    fCancelButton.reset (new TextButton ("cancel button"));
    addAndMakeVisible (fCancelButton.get());
    fCancelButton->setButtonText (TRANS("Cancel"));
    fCancelButton->addListener (this);
    fCancelButton->setColour (TextButton::buttonColourId, Colour (0x00a45c94));
    fCancelButton->setColour (TextButton::buttonOnColourId, Colour (0x00181f22));
    fCancelButton->setColour (TextButton::textColourOffId, Colour (0xff787878));

    drawable1 = Drawable::createFromImageData (BinaryData::iconcheckcolor_png, BinaryData::iconcheckcolor_pngSize);

    //[UserPreSize]
    //[/UserPreSize]

    setSize (683, 510);


    //[Constructor] You can add your own custom stuff here..

   fTagNameTextBox->setTextToShowWhenEmpty(TRANS("e.g. Cymbals"), AAppLookAndFeel::kMediumGray);

   fButtonLookAndFeel = std::make_unique<AButtonLookAndFeel>();
   fAddButton->setLookAndFeel(fButtonLookAndFeel.get());
   fAddMoreFilesButton->setLookAndFeel(fButtonLookAndFeel.get());
   fSaveButton->setLookAndFeel(fButtonLookAndFeel.get());

   fTextEditLookAndFeel = std::make_unique<ATextEditLookAndFeel>();
   fTagNameTextBox->setLookAndFeel(fTextEditLookAndFeel.get());
   fTagNameTextBox->addListener(this);

   // Prevent outline from being drawn
   fAddExamplesText->setEnabled(false);

   fFileTable->SetRemoveCallback([this]()
    {
       this->UpdateGui(fFileTable->getNumRows());
    });

   this->setWantsKeyboardFocus(true);

   this->ResetGui();

   fDisableTagButton->setVisible(false);
   fDeleteButton->setVisible(false);

    //[/Constructor]
}

AAddTagsView::~AAddTagsView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fFileTable = nullptr;
    fEmptyFileView = nullptr;
    fAlsoDragLabel = nullptr;
    fTagNameLabel = nullptr;
    fTagInfoView = nullptr;
    fTagsLabel = nullptr;
    fTagNameTextBox = nullptr;
    fDisableTagButton = nullptr;
    fSaveButton = nullptr;
    fAddButton = nullptr;
    fDeleteButton = nullptr;
    fAddMoreFilesButton = nullptr;
    fAddExamplesText = nullptr;
    fCancelButton = nullptr;
    drawable1 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void AAddTagsView::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff222222));

    {
        float x = static_cast<float> ((getWidth() / 2) - (683 / 2)), y = static_cast<float> ((getHeight() / 2) - (510 / 2)), width = 683.0f, height = 510.0f;
        Colour fillColour = Colour (0xffececec);
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (fillColour);
        g.fillRoundedRectangle (x, y, width, height, 20.000f);
    }

    {
        int x = 239, y = 104, width = 32, height = 32;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (Colours::black);
        jassert (drawable1 != 0);
        if (drawable1 != 0)
            drawable1->drawWithin (g, Rectangle<float> (x, y, width, height),
                                   RectanglePlacement::stretchToFit, 1.000f);
    }

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void AAddTagsView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fFileTable->setBounds (getWidth() - 26 - 340, 18, 340, 467);
    fEmptyFileView->setBounds (getWidth() - 26 - 340, 18, 340, 467);
    fTagNameTextBox->setBounds (20 + 81, 60 + 20 / 2 + 4 - (30 / 2), 173, 30);
    fSaveButton->setBounds (21, getHeight() - 24 - 50, 131, 50);
    fCancelButton->setBounds (170, (getHeight() - 24 - 50) + 0, 130, roundToInt (50 * 1.0000f));
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void AAddTagsView::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == fDisableTagButton.get())
    {
        //[UserButtonCode_fDisableTagButton] -- add your button handler code here..
        //[/UserButtonCode_fDisableTagButton]
    }
    else if (buttonThatWasClicked == fSaveButton.get())
    {
        //[UserButtonCode_fSaveButton] -- add your button handler code here..

       this->Exit(AddTags::kSave);

        //[/UserButtonCode_fSaveButton]
    }
    else if (buttonThatWasClicked == fAddButton.get())
    {
        //[UserButtonCode_fAddButton] -- add your button handler code here..

       this->ChooseAndAddFiles();

        //[/UserButtonCode_fAddButton]
    }
    else if (buttonThatWasClicked == fDeleteButton.get())
    {
        //[UserButtonCode_fDeleteButton] -- add your button handler code here..
        //[/UserButtonCode_fDeleteButton]
    }
    else if (buttonThatWasClicked == fAddMoreFilesButton.get())
    {
        //[UserButtonCode_fAddMoreFilesButton] -- add your button handler code here..

       this->ChooseAndAddFiles();

        //[/UserButtonCode_fAddMoreFilesButton]
    }
    else if (buttonThatWasClicked == fCancelButton.get())
    {
        //[UserButtonCode_fCancelButton] -- add your button handler code here..

       this->Exit(AddTags::kCancel);

        //[/UserButtonCode_fCancelButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

bool AAddTagsView::keyPressed (const KeyPress& key)
{
    //[UserCode_keyPressed] -- Add your code here...
   bool handled = false;
   if (key.isKeyCode(KeyPress::escapeKey))
   {
      handled = true;
      this->Exit(AddTags::kCancel);
   }
   return handled;
    //[/UserCode_keyPressed]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

bool AAddTagsView::AskToChangeTagNameTo(const String& name) const
{
   return AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
                                       TRANS("Change Tag Name?"),
                                       TRANS("Do you want to change the tag name to ") + name + "?",
                                       TRANS("Yes"), TRANS("No"));
}


void AAddTagsView::AddFiles(const Array<File>& files)
{
   if (files.size() == 1 && files[0].isDirectory())
   {
      const auto& name = files[0].getFileName();
      if (fTagNameTextBox->isEmpty() || this->AskToChangeTagNameTo(name))
      {
         fTagNameTextBox->setText(name);
      }
   }

   fFileTable->AddFiles(Filter(Flatten(files, 1),
                               ATagger::CanTagFile));

   this->UpdateGui(fFileTable->getNumRows());
}


void AAddTagsView::ChooseAndAddFiles()
{
   File initial;

   // Turn the list of extensions into a list of wildcards with extensions.
   String filePatterns = "*";
   filePatterns.append(ATagger::AudioFileTypes().joinIntoString(";*"), 1024);

   FileChooser chooser("***Title?***", initial, filePatterns, true, false, this);
   if (chooser.browseForMultipleFilesToOpen())
   {
      this->AddFiles(chooser.getResults());
   }
}


void AAddTagsView::Clear()
{
   fTagNameTextBox->clear();
   fTagNameTextBox->setEnabled(true);
   fFileTable->Clear();
   this->ResetGui();
}


void AAddTagsView::Exit(AddTags::Command command)
{
   if (AddTags::kCancel == command)
   {
      this->Clear();
   }
   this->getParentComponent()->postCommandMessage((int)command);
}


const Array<File>& AAddTagsView::Files() const
{
   return fFileTable->Files();
}


void AAddTagsView::HandleEmptyFileList()
{
   const bool isEmpty = (0 == fFileTable->getNumRows());
   fFileTable->setVisible(!isEmpty);
   fEmptyFileView->setVisible(isEmpty);
}


void AAddTagsView::ResetGui()
{
   fAddExamplesText->setText(TRANS("Add examples for this tag."));
   fAddExamplesText->setBounds(24, 101, 209, 20);

   fAddMoreFilesButton->setVisible(false);
   fDisableTagButton->setVisible(false);
   fDeleteButton->setVisible(false);
   fTagInfoView->setVisible(false);

   fAlsoDragLabel->setVisible(true);
   fAddButton->setVisible(true);

   // Until some examples are given, there is no saving.
   fSaveButton->setEnabled(false);

   this->HandleEmptyFileList();
}


void AAddTagsView::SetFiles(const Array<File>& files)
{
   fFileTable->Clear();
   this->AddFiles(files);
}

void AAddTagsView::SetFiles(const Array<File>&& files)
{
   fFileTable->Clear();
   this->AddFiles(std::move(files));
}

void AAddTagsView::SetTagInfo(const ATag& tag)
{
   fTagNameTextBox->setText(tag.Name());
   fTagNameTextBox->setEnabled(tag.Name().empty());
}

String AAddTagsView::TagName() const
{
   return fTagNameTextBox->getText();
}


void AAddTagsView::UpdateGui(int fileCount)
{
   // Update the GUI
   String addedMessage = "Added ";
   addedMessage << fileCount << " to " << this->fTagNameTextBox->getText();
   fAddExamplesText->setText(addedMessage);

   auto textBounds = fAddExamplesText->getBounds();
   textBounds.setY(111);
   fAddExamplesText->setBounds(textBounds);

   fTagInfoView->SetStrength(fileCount);

   fAddMoreFilesButton->setVisible(true);
   fDisableTagButton->setVisible(true);

#if 0
   fDeleteButton->setVisible(true);
   fTagInfoView->setVisible(true);
#endif

   fAlsoDragLabel->setVisible(false);
   fAddButton->setVisible(false);

   this->UpdateSaveButton();
   this->HandleEmptyFileList();
}


void AAddTagsView::UpdateSaveButton()
{
   fSaveButton->setEnabled(fFileTable->FileCount() > 0 && !fTagNameTextBox->isEmpty());
}

// MARK: DragAndDropTarget interface

void AAddTagsView::filesDropped(const StringArray& filenames, int mouseX, int mouseY)
{
   Array<File> files;
   StringArray badFiles;
   for (const auto& filename : filenames)
   {
      File file { filename };
      if (ATagger::CanTagFile(file) || file.isDirectory())
      {
         files.add(file);
      }
      else
      {
         badFiles.add(file.getFileName());
      }
   }

   this->AddFiles(files);

   if (!badFiles.isEmpty())
   {
      AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon,
       TRANS("Unsupported Files"),
       TRANS("The following files were not added\n") + badFiles.joinIntoString(", "));
   }
}


bool AAddTagsView::isInterestedInFileDrag(const StringArray& files)
{
   for (File file : files)
   {
      if (ATagger::CanTagFile(file) || file.isDirectory())
      {
         return true;
      }
   }
   return false;
}


// MARK: TextEditor::Listener interface

void AAddTagsView::textEditorTextChanged(TextEditor& textEditor)
{
   this->UpdateSaveButton();
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="AAddTagsView" componentName=""
                 parentClasses="public Component, public FileDragAndDropTarget, public TextEditor::Listener"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="1" initialWidth="683"
                 initialHeight="510">
  <METHODS>
    <METHOD name="keyPressed (const KeyPress&amp; key)"/>
  </METHODS>
  <BACKGROUND backgroundColour="ff222222">
    <ROUNDRECT pos="0Cc 0Cc 683 510" cornerSize="20.0" fill="solid: ffececec"
               hasStroke="0"/>
    <IMAGE pos="239 104 32 32" resource="BinaryData::iconcheckcolor_png"
           opacity="1.0" mode="0"/>
  </BACKGROUND>
  <GENERICCOMPONENT name="file table" id="1a6a7815334d90e3" memberName="fFileTable"
                    virtualName="" explicitFocusOrder="3" pos="26Rr 18 340 467" class="ASimpleFileTable"
                    params=""/>
  <GENERICCOMPONENT name="empty file view" id="90466f16ad0f01f0" memberName="fEmptyFileView"
                    virtualName="" explicitFocusOrder="3" pos="26Rr 18 340 467" class="AEmptyAddTagFileView"
                    params=""/>
  <LABEL name="also drag label" id="fcfff2f27303c216" memberName="fAlsoDragLabel"
         virtualName="" explicitFocusOrder="0" pos="24 119 209 20" textCol="ff666666"
         edTextCol="ff000000" edBkgCol="0" labelText="You can also drag and drop."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default sans-serif font" fontsize="13.0" kerning="0.0"
         bold="0" italic="1" justification="33" typefaceStyle="Italic"/>
  <LABEL name="tag name label" id="7fc5972ea90a3d45" memberName="fTagNameLabel"
         virtualName="" explicitFocusOrder="0" pos="20 60 93 20" textCol="ff666666"
         edTextCol="ff000000" edBkgCol="0" labelText="Tag name:" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="16.0" kerning="0.0" bold="0" italic="0" justification="33"/>
  <GENERICCOMPONENT name="tag info view" id="c07d3d3463039d96" memberName="fTagInfoView"
                    virtualName="" explicitFocusOrder="0" pos="14 283 298 128" class="AAddTagInfoView"
                    params=""/>
  <LABEL name="tags label" id="931523bef18819ca" memberName="fTagsLabel"
         virtualName="" explicitFocusOrder="0" pos="21 14 213 20" textCol="ff666666"
         edTextCol="ff000000" edBkgCol="0" labelText="Tags" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default sans-serif font"
         fontsize="17.0" kerning="0.0" bold="1" italic="0" justification="33"
         typefaceStyle="Bold"/>
  <TEXTEDITOR name="tag name text box" id="f2e2f21cb511b507" memberName="fTagNameTextBox"
              virtualName="" explicitFocusOrder="1" pos="81 4Cc 173 30" posRelativeX="7fc5972ea90a3d45"
              posRelativeY="7fc5972ea90a3d45" textcol="ff666666" bkgcol="ffececec"
              outlinecol="ffc8c8c8" caretcol="ffececec" initialText="" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="0" caret="1" popupmenu="0"/>
  <TOGGLEBUTTON name="disable tag button" id="ae4a8544d6316c92" memberName="fDisableTagButton"
                virtualName="" explicitFocusOrder="5" pos="20 224 150 27" txtcol="ff666666"
                buttonText="Disable this tag" connectedEdges="0" needsCallback="1"
                radioGroupId="0" state="0"/>
  <TEXTBUTTON name="save button" id="8d22f7eb60c074d4" memberName="fSaveButton"
              virtualName="" explicitFocusOrder="4" pos="21 24Rr 131 50" textCol="ffdee1df"
              buttonText="Save Changes" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTBUTTON name="add button" id="80808378897bfb1c" memberName="fAddButton"
              virtualName="" explicitFocusOrder="2" pos="236 101 38 38" textCol="ffebebeb"
              buttonText="+" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="delete button" id="3610551dc58b7512" memberName="fDeleteButton"
              virtualName="" explicitFocusOrder="0" pos="171 224 112 27" bgColOff="a45c94"
              bgColOn="181f22" textCol="fff6241f" textColOn="fff6241f" buttonText="Delete this tag"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="add more files button" id="7754d4cd17a163db" memberName="fAddMoreFilesButton"
              virtualName="" explicitFocusOrder="4" pos="21 160 254 50" textCol="ffdee1df"
              buttonText="Add More Files" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
  <TEXTEDITOR name="add examples text" id="6464bfee90970107" memberName="fAddExamplesText"
              virtualName="" explicitFocusOrder="0" pos="24 101 209 20" textcol="ff666666"
              bkgcol="495358" initialText="Add examples for this tag." multiline="0"
              retKeyStartsLine="0" readonly="1" scrollbars="0" caret="0" popupmenu="0"/>
  <TEXTBUTTON name="cancel button" id="1dd15879622d3e8f" memberName="fCancelButton"
              virtualName="" explicitFocusOrder="0" pos="170 0 130 100%" posRelativeY="8d22f7eb60c074d4"
              posRelativeH="8d22f7eb60c074d4" bgColOff="a45c94" bgColOn="181f22"
              textCol="ff787878" buttonText="Cancel" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

