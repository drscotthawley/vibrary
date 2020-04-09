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

#pragma once

//[Headers]     -- You can add your own extra header files here --
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

#include <map>

#include "../JuceLibraryCode/JuceHeader.h"

#include "ContentView.h"
#include "TagView.h"
#include "TrainingManager.h"

class AAddTagsOverlay;
class AButtonLookAndFeel;
class SortFileComboBox;

//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class TrainingView  : public ContentView,
                      public ComboBox::Listener,
                      public AsyncUpdater,
                      public Button::Listener,
                      public Slider::Listener
{
public:
    //==============================================================================
    TrainingView ();
    ~TrainingView();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

   void comboBoxChanged(ComboBox* comboBoxThatChanged) override;
   void handleAsyncUpdate() override;

   // Handles AAddTagsOverlay::kDismissed
   void handleCommandMessage(int commandId) override;

    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void buttonClicked (Button* buttonThatWasClicked) override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void parentSizeChanged() override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
   void HandleSortFileComboBoxChange();
   bool ConfirmSortFileChange() const;
   void ShowAddTagsOverlay(const ATag& tag);
   void HandleAddTagsFinished(int byCommandId);
   void ProcessAddTagResults();
   void ShowTags(bool showTags);
   void Train();
   void FinishedTraining();

   /**
    * Request a name from the user. The new name is stored in fSortFile.
    * Returns true if training should continue, or false if the user has cancelled it.
    */
   bool AskForName();

   std::map<String, Array<File>> fExamples; //!< Tag name -> example files
   std::unique_ptr<AAddTagsOverlay> fAddTagsOverlay;
   std::unique_ptr<AButtonLookAndFeel> fButtonLookAndFeel;
   std::unique_ptr<DialogWindow> fNameDialog;
   ATrainingManager fTrainingManager;
   String fSortFileName;
   int fSelectedComboBoxId;
   Image fNextImage;
   String fNextMessage;
   String fDefaultMessage;
   int fTrainingProgress = 0;
   bool fTraining = false;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<Label> fMessageLabel;
    std::unique_ptr<ATagView> fTagView;
    std::unique_ptr<TextButton> fCreateTagsButton;
    std::unique_ptr<TextButton> fTrainButton;
    std::unique_ptr<Label> fTrainExplanation;
    std::unique_ptr<Label> fCreateExplanation;
    std::unique_ptr<SortFileComboBox> fSortFileComboBox;
    std::unique_ptr<Slider> fProgressSlider;
    std::unique_ptr<ImageComponent> fSpectraImage;
    std::unique_ptr<Drawable> drawable1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrainingView)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

