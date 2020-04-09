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

#include <iostream>
#include <iterator>
#include <memory>
#include <thread>

#include "AddTagsOverlay.h"
#include "AddTagsView.h"
#include "ButtonLookAndFeel.h"
#include "CommandMessages.h"
#include "NameSortFile.h"
#include "SortFileComboBox.h"
#include "Tag.h"

//[/Headers]

#include "TrainingView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
namespace
{
  //!!! constexpr const char* const kDefaultTagName = "Unnamed Tag";
}
//[/MiscUserDefs]

//==============================================================================
TrainingView::TrainingView ()
{
    //[Constructor_pre] You can add your own custom stuff here..

   fSortFileName = TRANS(ATrainingManager::kDefaultSortFileName);

    //[/Constructor_pre]

    fMessageLabel.reset (new Label ("message label",
                                    TRANS("Manage your tags and train your Vibrary to begin sorting.")));
    addAndMakeVisible (fMessageLabel.get());
    fMessageLabel->setFont (Font (Font::getDefaultSansSerifFontName(), 19.00f, Font::plain).withTypefaceStyle ("Regular"));
    fMessageLabel->setJustificationType (Justification::centred);
    fMessageLabel->setEditable (false, false, false);
    fMessageLabel->setColour (Label::backgroundColourId, Colour (0x00000000));
    fMessageLabel->setColour (Label::textColourId, Colour (0xff666666));
    fMessageLabel->setColour (TextEditor::textColourId, Colours::black);
    fMessageLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fTagView.reset (new ATagView());
    addAndMakeVisible (fTagView.get());
    fTagView->setName ("tag view");

    fCreateTagsButton.reset (new TextButton ("create tags button"));
    addAndMakeVisible (fCreateTagsButton.get());
    fCreateTagsButton->setButtonText (TRANS("Create Tags"));
    fCreateTagsButton->addListener (this);
    fCreateTagsButton->setColour (TextButton::buttonColourId, Colour (0xff00a6ff));
    fCreateTagsButton->setColour (TextButton::buttonOnColourId, Colour (0xffa319b0));
    fCreateTagsButton->setColour (TextButton::textColourOffId, Colour (0xffdadada));

    fTrainButton.reset (new TextButton ("train button"));
    addAndMakeVisible (fTrainButton.get());
    fTrainButton->setButtonText (TRANS("Train your Vibrary"));
    fTrainButton->addListener (this);
    fTrainButton->setColour (TextButton::textColourOffId, Colour (0xffdadada));

    fTrainExplanation.reset (new Label ("train explanation",
                                        TRANS("Once you are satisfied with your tags,\n"
                                        "you can train your Vibrary.")));
    addAndMakeVisible (fTrainExplanation.get());
    fTrainExplanation->setFont (Font (Font::getDefaultSansSerifFontName(), 17.00f, Font::plain).withTypefaceStyle ("Regular"));
    fTrainExplanation->setJustificationType (Justification::centred);
    fTrainExplanation->setEditable (false, false, false);
    fTrainExplanation->setColour (Label::textColourId, Colour (0xff666666));
    fTrainExplanation->setColour (TextEditor::textColourId, Colours::black);
    fTrainExplanation->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fCreateExplanation.reset (new Label ("create expanation",
                                         TRANS("Create tags - These will be used to sort your Vibrary.")));
    addAndMakeVisible (fCreateExplanation.get());
    fCreateExplanation->setFont (Font (Font::getDefaultSansSerifFontName(), 17.00f, Font::plain).withTypefaceStyle ("Regular"));
    fCreateExplanation->setJustificationType (Justification::centred);
    fCreateExplanation->setEditable (false, false, false);
    fCreateExplanation->setColour (Label::textColourId, Colour (0xff666666));
    fCreateExplanation->setColour (TextEditor::textColourId, Colours::black);
    fCreateExplanation->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    fSortFileComboBox.reset (new SortFileComboBox());
    addAndMakeVisible (fSortFileComboBox.get());
    fSortFileComboBox->setName ("sort file combo box");

    fProgressSlider.reset (new Slider ("progress slider"));
    addAndMakeVisible (fProgressSlider.get());
    fProgressSlider->setRange (0, 100, 0);
    fProgressSlider->setSliderStyle (Slider::LinearHorizontal);
    fProgressSlider->setTextBoxStyle (Slider::NoTextBox, true, 0, 0);
    fProgressSlider->setColour (Slider::backgroundColourId, Colours::white);
    fProgressSlider->setColour (Slider::thumbColourId, Colour (0x0030aade));
    fProgressSlider->setColour (Slider::trackColourId, Colour (0xff30aade));
    fProgressSlider->addListener (this);

    fSpectraImage.reset (new ImageComponent());
    addAndMakeVisible (fSpectraImage.get());
    fSpectraImage->setName ("new component");

    drawable1 = Drawable::createFromImageData (BinaryData::imagetrain_png, BinaryData::imagetrain_pngSize);

    //[UserPreSize]

   fProgressSlider->setVisible(false);
   fSpectraImage->setVisible(false);

    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..

   fDefaultMessage = fMessageLabel->getText();

   fSelectedComboBoxId = fSortFileComboBox->getSelectedId();
   fSortFileComboBox->addListener(this);

   fButtonLookAndFeel = std::make_unique<AButtonLookAndFeel>();
   fCreateTagsButton->setLookAndFeel(fButtonLookAndFeel.get());
   fTrainButton->setLookAndFeel(fButtonLookAndFeel.get());
   fTrainButton->setEnabled(false);

   fTagView->SetTagFunction([this](const ATag& tag)
    {
       this->ShowAddTagsOverlay(tag);
    });

   this->ShowTags(false);

    //[/Constructor]
}

TrainingView::~TrainingView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..

   if (fNameDialog)
   {
      fNameDialog->exitModalState(0);
   }

    //[/Destructor_pre]

    fMessageLabel = nullptr;
    fTagView = nullptr;
    fCreateTagsButton = nullptr;
    fTrainButton = nullptr;
    fTrainExplanation = nullptr;
    fCreateExplanation = nullptr;
    fSortFileComboBox = nullptr;
    fProgressSlider = nullptr;
    fSpectraImage = nullptr;
    drawable1 = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void TrainingView::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0x50111111));

    {
        int x = (getWidth() / 2) - (250 / 2), y = 2, width = 250, height = 72;
        //[UserPaintCustomArguments] Customize the painting arguments here..
        //[/UserPaintCustomArguments]
        g.setColour (Colours::black);
        jassert (drawable1 != 0);
        if (drawable1 != 0)
            drawable1->drawWithin (g, Rectangle<float> (x, y, width, height),
                                   RectanglePlacement::centred, 1.000f);
    }

    //[UserPaint] Add your own custom painting code here..
   g.setColour(Colour(0x66, 0x66, 0x66));
   g.drawLine(0.0f, getHeight() - 66, getWidth(), getHeight() - 66);
    //[/UserPaint]
}

void TrainingView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fMessageLabel->setBounds (0, getHeight() - 70, proportionOfWidth (1.0000f), 70);
    fTagView->setBounds (0, 163, proportionOfWidth (1.0000f), proportionOfHeight (0.4656f));
    fCreateTagsButton->setBounds ((getWidth() / 2) - (231 / 2), 163, 231, 50);
    fTrainButton->setBounds ((getWidth() / 2) - (231 / 2), getHeight() - 99 - 50, 231, 50);
    fTrainExplanation->setBounds ((getWidth() / 2) - (541 / 2), (getHeight() - 99 - 50) + -14 - 32, 541, 32);
    fCreateExplanation->setBounds ((getWidth() / 2) - (451 / 2), 80, 451, 16);
    fSortFileComboBox->setBounds ((getWidth() / 2) - (231 / 2), 115, 231, 32);
    fProgressSlider->setBounds (0, getHeight() - 61 - 10, proportionOfWidth (1.0000f), 10);
    fSpectraImage->setBounds (((getWidth() / 2) - (231 / 2)) + 258, (getHeight() - 99 - 50) + 2, 81, 50 - 0);
    //[UserResized] Add your own custom resize handling here..

   // Make height of tag view relative to where train explanation text was placed.
   fTagView->setBounds(0, 163, proportionOfWidth(1.0f), fTrainExplanation->getY() - 163 - 10);

    //[/UserResized]
}

void TrainingView::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == fCreateTagsButton.get())
    {
        //[UserButtonCode_fCreateTagsButton] -- add your button handler code here..

       fMessageLabel->setText(fDefaultMessage, dontSendNotification);
       this->ShowAddTagsOverlay(ATag());

        //[/UserButtonCode_fCreateTagsButton]
    }
    else if (buttonThatWasClicked == fTrainButton.get())
    {
        //[UserButtonCode_fTrainButton] -- add your button handler code here..

       this->Train();

        //[/UserButtonCode_fTrainButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void TrainingView::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == fProgressSlider.get())
    {
        //[UserSliderCode_fProgressSlider] -- add your slider handling code here..
        //[/UserSliderCode_fProgressSlider]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void TrainingView::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
   this->setSize(this->getParentWidth(), this->getParentHeight());
    //[/UserCode_parentSizeChanged]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void TrainingView::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
   if (comboBoxThatHasChanged == fSortFileComboBox.get())
   {
      this->HandleSortFileComboBoxChange();
   }
}

void TrainingView::handleAsyncUpdate()
{
   fProgressSlider->setValue(fTrainingProgress);
   fSpectraImage->setImage(fNextImage);
   if (fNextMessage != fMessageLabel->getText())
   {
      fMessageLabel->setText(fNextMessage, dontSendNotification);
   }
}

void TrainingView::handleCommandMessage(int commandId)
{
   switch (commandId)
   {
      case Overlay::kAccepted:
      case Overlay::kDismissed:
      this->HandleAddTagsFinished(commandId);
      break;

      case AddTags::kAddTag:
      this->ShowAddTagsOverlay(ATag());
      break;

      case Training::kSortFileAdded:
      fSortFileComboBox->Populate(true);
      break;

      case Training::kTrainingDone:
      this->FinishedTraining();
      break;
   }
}

bool TrainingView::AskForName()
{
   // Check if somehow a training was already started.
   if (fNameDialog)
   {
      return false; // EARLY EXIT
   }

   int result = ANameSortFile::kCancelled;

   // We wouldn't show this if we are editing an existing sort file.
   bool named = false;
   auto newName = fSortFileName;

   auto content = std::make_unique<ANameSortFile>();
   content->SetName(newName);

   DialogWindow::LaunchOptions lo;
   lo.dialogTitle = "Name";
   lo.content.setNonOwned(content.get());
   lo.useNativeTitleBar = false;
   lo.componentToCentreAround = this->getTopLevelComponent();
   lo.escapeKeyTriggersCloseButton = true;
   lo.resizable = false;

   fNameDialog.reset(lo.create());
   fNameDialog->setVisible(true);

   result = fNameDialog->runModalLoop();

   fNameDialog.reset();

   if (ANameSortFile::kOkay == result)
   {
      newName = content->Name();
      if (newName.isNotEmpty())
      {
         // Validate the name which has to be a valid filename and shouldn't
         // be able to be interpreted as a path.
         newName = newName.replace("/", "");

         // This also gets rid of things that might be used for SQL injection ';'.
         newName = File::createLegalPathName(newName);
         named = !File("./" + newName).getFileName().isEmpty();

         fSortFileName = named ? newName : ATrainingManager::kDefaultSortFileName;
      }
   }

   return named;
}


bool TrainingView::ConfirmSortFileChange() const
{
   bool allow = true;
   if (!fExamples.empty())
   {
      const int answer = AlertWindow::showOkCancelBox(AlertWindow::QuestionIcon,
       TRANS("Lose Your Work?"),
       TRANS("You have already created some tags, and you will lose your work. "
             "Create a new sort file anyway?"),
       TRANS("YES"), TRANS("NO"));

      allow = (1 == answer);
   }
   return allow;
}


void TrainingView::FinishedTraining()
{
   fExamples.clear();
   fMainView->postCommandMessage(Training::kSortFileAdded);

   fSpectraImage->setImage(ImageCache::getFromMemory(BinaryData::iconcheckcolor_png,
                                                     BinaryData::iconcheckcolor_pngSize));

   // Are we going to be stuck with this if there is an exception?
   MouseCursor::hideWaitCursor();

   fMainView->postCommandMessage(Training::kTrainingDone);
   fTraining = false;
}


void TrainingView::HandleAddTagsFinished(int commandId)
{
   if (Overlay::kAccepted == commandId)
   {
      this->ProcessAddTagResults();
   }
   this->ShowTags(fTagView->TagsSize() > 0UL);
   fTrainButton->setEnabled(!fExamples.empty() && 2 <= fTagView->TagsSize());
}


void TrainingView::HandleSortFileComboBoxChange()
{
#if kAllowRetraining

   const int selectedId = fSortFileComboBox->getSelectedId();
   if (this->ConfirmSortFileChange())
   {
      fSelectedComboBoxId = selectedId;
      fExamples.clear();

      if (fSortFileComboBox->CreateItemId() == selectedId)
      {
         fSortFileName = TRANS(ATrainingManager::kDefaultSortFileName);
         fTagView->SetTags({});
         this->ShowTags(false);
         this->ShowAddTagsOverlay(ATag());
         fMessageLabel->setText(fDefaultMessage, dontSendNotification);
      }
      else
      {
         fSortFileName = fSortFileComboBox->SelectedSortFile().Name();
         fTagView->SetTagsFrom(fSortFileComboBox->SelectedSortFile());
         this->ShowTags(true);
      }
   }
   else
   {
      fSortFileComboBox->setSelectedId(fSelectedComboBoxId, dontSendNotification);
   }

#else
   
   AlertWindow::showMessageBox(AlertWindow::WarningIcon,
   TRANS("Retraining Is Not Supported Yet"),
   TRANS("Vibrary does not have retraining of an existing sort file implemented yet."));

#endif
}


void TrainingView::ProcessAddTagResults()
{
   auto tagName { fAddTagsOverlay->TagName() };
   auto tagIt { fExamples.find(tagName) };
   if (tagIt != fExamples.end())
   {
      Array<File> temp = tagIt->second;
      temp.addArray(fAddTagsOverlay->Files());
      auto uniqueEnd = std::unique(temp.begin(), temp.end());
      temp.resize(static_cast<int>(std::distance(temp.begin(), uniqueEnd)));

      tagIt->second = std::move(temp);

      // TODO: Update tag button with new count
   }
   else
   {
      fTagView->AddTag({ tagName.toStdString() });
      fExamples[tagName] = fAddTagsOverlay->Files();
   }
}


void TrainingView::ShowAddTagsOverlay(const ATag& tag)
{
   if (!fAddTagsOverlay)
   {
      fAddTagsOverlay = std::make_unique<AAddTagsOverlay>();
   }

   fAddTagsOverlay->Clear();
   fAddTagsOverlay->SetTagInfo(tag);

   auto tagIt { fExamples.find(String(tag.Name())) };
   if (fExamples.end() != tagIt)
   {
      // This is a newly created tag, so allow user to edit the name.
      fAddTagsOverlay->SetFiles(tagIt->second);
   }

   fAddTagsOverlay->ShowIn(this, this->getTopLevelComponent());
   fAddTagsOverlay->grabKeyboardFocus();
}


void TrainingView::ShowTags(bool doShowTags)
{
   fTagView->setVisible(doShowTags);
   fCreateTagsButton->setVisible(!doShowTags);
}


void TrainingView::Train()
{
   if (!this->AskForName())
   {
      AlertWindow::showMessageBox(AlertWindow::InfoIcon,
                                  TRANS("Training Canceled"),
                                  TRANS("Adjust your examples as needed and "
                                        "enter a name when prompted."));
      return;
   }

   fTraining = true;
   MouseCursor::showWaitCursor();
   fSpectraImage->setVisible(true);
   fTrainingProgress = 0;
   fProgressSlider->setValue(0.0);
   fProgressSlider->setVisible(true);

   fTrainingManager.SetSortFileName(fSortFileName);

   fTrainingManager.LaunchTraining(fExamples,
    [this](int completed, const char* msg, const juce::Image& spectraImage)
    {
       fTrainingProgress = completed;

       if (completed >= 100 && fTraining)
       {
          this->postCommandMessage(Training::kTrainingDone);
       }

       if (nullptr != msg)
       {
          if (completed < 0)
          {
             AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                              TRANS("Training Error"),
                                              String(msg));
          }
          else
          {
             fNextMessage = msg;
          }
      }

      if (spectraImage.isValid())
      {
         fNextImage = spectraImage;
      }

      if (nullptr != msg)
      {
         std::cerr << msg;
      }

      this->triggerAsyncUpdate();
    });
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TrainingView" componentName=""
                 parentClasses="public ContentView, public ComboBox::Listener, public AsyncUpdater"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
  </METHODS>
  <BACKGROUND backgroundColour="50111111">
    <IMAGE pos="0Cc 2 250 72" resource="BinaryData::imagetrain_png" opacity="1.0"
           mode="1"/>
  </BACKGROUND>
  <LABEL name="message label" id="f31f40d179024a63" memberName="fMessageLabel"
         virtualName="" explicitFocusOrder="0" pos="0 0Rr 100% 70" bkgCol="0"
         textCol="ff666666" edTextCol="ff000000" edBkgCol="0" labelText="Manage your tags and train your Vibrary to begin sorting."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default sans-serif font" fontsize="19.0" kerning="0.0"
         bold="0" italic="0" justification="36"/>
  <GENERICCOMPONENT name="tag view" id="a673a185665c3e7f" memberName="fTagView" virtualName=""
                    explicitFocusOrder="0" pos="0 163 100% 46.562%" class="ATagView"
                    params=""/>
  <TEXTBUTTON name="create tags button" id="7462ac50b130c105" memberName="fCreateTagsButton"
              virtualName="" explicitFocusOrder="0" pos="0.5Cc 163 231 50"
              bgColOff="ff00a6ff" bgColOn="ffa319b0" textCol="ffdadada" buttonText="Create Tags"
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="train button" id="80dc3e04660f8b5f" memberName="fTrainButton"
              virtualName="" explicitFocusOrder="0" pos="0.5Cc 99Rr 231 50"
              textCol="ffdadada" buttonText="Train your Vibrary" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <LABEL name="train explanation" id="66e5184408a9c37e" memberName="fTrainExplanation"
         virtualName="" explicitFocusOrder="0" pos="-0.5Cc -14r 541 32"
         posRelativeY="80dc3e04660f8b5f" textCol="ff666666" edTextCol="ff000000"
         edBkgCol="0" labelText="Once you are satisfied with your tags,&#10;you can train your Vibrary."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default sans-serif font" fontsize="17.0" kerning="0.0"
         bold="0" italic="0" justification="36"/>
  <LABEL name="create expanation" id="7f19d013d976ed7b" memberName="fCreateExplanation"
         virtualName="" explicitFocusOrder="0" pos="0.5Cc 80 451 16" textCol="ff666666"
         edTextCol="ff000000" edBkgCol="0" labelText="Create tags - These will be used to sort your Vibrary."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default sans-serif font" fontsize="17.0" kerning="0.0"
         bold="0" italic="0" justification="36"/>
  <GENERICCOMPONENT name="sort file combo box" id="a4295bddcee15d40" memberName="fSortFileComboBox"
                    virtualName="" explicitFocusOrder="0" pos="0.5Cc 115 231 32"
                    class="SortFileComboBox" params=""/>
  <SLIDER name="progress slider" id="805d1de70630e779" memberName="fProgressSlider"
          virtualName="" explicitFocusOrder="0" pos="0 61Rr 100% 10" bkgcol="ffffffff"
          thumbcol="30aade" trackcol="ff30aade" min="0.0" max="100.0" int="0.0"
          style="LinearHorizontal" textBoxPos="NoTextBox" textBoxEditable="0"
          textBoxWidth="0" textBoxHeight="0" skewFactor="1.0" needsCallback="1"/>
  <GENERICCOMPONENT name="new component" id="b23ab087490508d4" memberName="fSpectraImage"
                    virtualName="" explicitFocusOrder="0" pos="258 2 81 0M" posRelativeX="80dc3e04660f8b5f"
                    posRelativeY="80dc3e04660f8b5f" posRelativeH="80dc3e04660f8b5f"
                    class="ImageComponent" params=""/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

