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

#include <chrono>
#include <numeric>
#include <thread>

#include "ButtonLookAndFeel.h"
#include "CommandMessages.h"
#include "DetailFileTable.h"
#include "EmptyFileListView.h"
#include "FileToolBar.h"
#include "ProgressOverlay.h"

//[/Headers]

#include "FileListView.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
FileListView::FileListView ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    fEmptyView.reset (new AEmptyFileListView());
    addAndMakeVisible (fEmptyView.get());
    fEmptyView->setName ("empty view");

    fFileTable.reset (new ADetailFileTable());
    addAndMakeVisible (fFileTable.get());
    fFileTable->setName ("file table");

    fAddFilesButton.reset (new TextButton ("add files button"));
    addAndMakeVisible (fAddFilesButton.get());
    fAddFilesButton->setButtonText (TRANS("Add Files"));
    fAddFilesButton->addListener (this);
    fAddFilesButton->setColour (TextButton::buttonColourId, Colour (0x00a45c94));
    fAddFilesButton->setColour (TextButton::buttonOnColourId, Colour (0x00181f22));

    fToolBar.reset (new AFileToolBar());
    addAndMakeVisible (fToolBar.get());
    fToolBar->setName ("tool bar");

    fRemoveButton.reset (new TextButton ("remove button"));
    addAndMakeVisible (fRemoveButton.get());
    fRemoveButton->setButtonText (TRANS("Remove"));
    fRemoveButton->addListener (this);
    fRemoveButton->setColour (TextButton::buttonColourId, Colour (0x00a45c94));
    fRemoveButton->setColour (TextButton::buttonOnColourId, Colour (0x00181f22));
    fRemoveButton->setColour (TextButton::textColourOffId, Colour (0xffb4b8b5));


    //[UserPreSize]

   fButtonLookAndFeel = std::make_unique<AButtonLookAndFeel>();
   fButtonLookAndFeel->fCornerSize = 10.0f;
   fButtonLookAndFeel->fFontSizePixels = 16;
   fAddFilesButton->setLookAndFeel(fButtonLookAndFeel.get());

    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..

   // We probably want to page this instead of reading everything.
   fFileTable->LoadFiles(0, {}, false);
   this->HandleEmptyFileTable();

   fToolBar->addChangeListener(this);
   fFileTable->addChangeListener(this);

   fAddFilesButton->setEnabled(false);
   fRemoveButton->setEnabled(false);

    //[/Constructor]
}

FileListView::~FileListView()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    fEmptyView = nullptr;
    fFileTable = nullptr;
    fAddFilesButton = nullptr;
    fToolBar = nullptr;
    fRemoveButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void FileListView::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0x88111111));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void FileListView::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    fEmptyView->setBounds (11 + 0, 11 + 50, roundToInt ((getWidth() - 22) * 1.0000f), getHeight() - 110);
    fFileTable->setBounds (11 + 0, 11 + 50, roundToInt ((getWidth() - 22) * 1.0000f), getHeight() - 110);
    fAddFilesButton->setBounds (11 + 0, getHeight() - 35, 150, 32);
    fToolBar->setBounds (11, 11, getWidth() - 22, 50);
    fRemoveButton->setBounds ((11 + 0) + 150 - -10, (getHeight() - 35) + 32 / 2 - (32 / 2), 150, 32);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void FileListView::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == fAddFilesButton.get())
    {
        //[UserButtonCode_fAddFilesButton] -- add your button handler code here..

       this->ChooseAndAddFiles();

        //[/UserButtonCode_fAddFilesButton]
    }
    else if (buttonThatWasClicked == fRemoveButton.get())
    {
        //[UserButtonCode_fRemoveButton] -- add your button handler code here..

       this->RemoveSelectedFiles();

        //[/UserButtonCode_fRemoveButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}

void FileListView::parentSizeChanged()
{
    //[UserCode_parentSizeChanged] -- Add your code here...
   this->setSize(this->getParentWidth(), this->getParentHeight());
    //[/UserCode_parentSizeChanged]
}

void FileListView::filesDropped (const StringArray& filenames, int mouseX, int mouseY)
{
    //[UserCode_filesDropped] -- Add your code here...

   Array<File> files;
   for (const auto& filename : filenames)
   {
      files.add(filename);
   }
   this->AddFiles(std::move(files));

    //[/UserCode_filesDropped]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...

void FileListView::changeListenerCallback(ChangeBroadcaster* source)
{
   if (source == fToolBar.get())
   {
      if (fToolBar->SelectedSortFile().Key() >= 0)
      {
         fAddFilesButton->setEnabled(true);
      }
      this->ReloadFileTable();
      this->HandleEmptyFileTable();
   }
   else if (source == fFileTable.get())
   {
      fRemoveButton->setEnabled(0 < fFileTable->SelectionSize());
   }
}


void FileListView::handleCommandMessage(int commandId)
{
   switch(commandId)
   {
      case Files::kTaggingError:
      AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon,
                                       TRANS("Tagging Error"),
                                       fTaggingManager.LastErrorMessage());
      break;

      case Files::kReloadTable:
      this->ReloadFileTable();
      break;

      default:
      fToolBar->handleCommandMessage(commandId);
      fFileTable->handleCommandMessage(commandId);
   }
}


bool FileListView::isInterestedInFileDrag(const StringArray& files)
{
   for (File file : files)
   {
      if (ATaggingManager::CanTagFile(file) || file.isDirectory())
      {
         return true;
      }
   }
   return false;
}


void FileListView::AddFiles(const Array<File>& files)
{
   auto& sortFile = fToolBar->SelectedSortFile();
   if (sortFile.Name().empty())
   {
      AlertWindow::showMessageBox(AlertWindow::InfoIcon,
                                  TRANS("No Sort File Selected"),
                                  TRANS("First chose a sort file containing the tags you want to use "
                                        "before adding audio files."));
   }
   else
   {
      fTaggingManager.SetSortFile(sortFile);

      int expectedCount = std::accumulate(files.begin(), files.end(), 0,
       [](int total, const File& f)
       {
          return total + (f.existsAsFile() ? 1 : f.getNumberOfChildFiles(File::findFiles));
       });

      if (nullptr == fProgressOverlay.get())
      {
         fProgressOverlay = std::make_unique<AProgressOverlay>();
      }
      fProgressOverlay->SetFileCount(expectedCount); // updates in progres func
      fProgressOverlay->ShowIn(this, this->getTopLevelComponent());

      std::thread([files, expectedCount, this]()
       {
         int addedCount = 0;
         try
         {
             addedCount = fTaggingManager.TagAndAddFiles(files,
              [this](int /*completed*/, int total, const File& spectraImageFile)
              {
                 fProgressOverlay->SetFileCount(total);
                 fProgressOverlay->SetNextImageFile(spectraImageFile);
                 this->postCommandMessage(Files::kReloadTable);
              });
         }
         catch (const std::exception& ex)
         {
            // TODO: display error msg
            std::cerr << ex.what() << "\n";
         }

          fProgressOverlay->SetNextImage(ImageCache::getFromMemory(BinaryData::iconcheckwhite2x_png,
                                                                   BinaryData::iconcheckwhite2x_pngSize));
          std::this_thread::sleep_for(std::chrono::seconds(1));

          fProgressOverlay->AsyncExit();

          if (expectedCount != addedCount)
          {
             this->postCommandMessage(Files::kTaggingError);
          }

          this->postCommandMessage(Files::kReloadTable);

       }).detach(); // $$$: GSL says don't use detach this way
   }
}


void FileListView::ChooseAndAddFiles()
{
   File initial;
   String filePatterns; // TODO
   FileChooser chooser(TRANS("Select Audio Files To Add"), initial, filePatterns,
                       true, false, this);
   if (chooser.browseForMultipleFilesToOpen())
   {
      this->AddFiles(chooser.getResults());
   }
}


void FileListView::HandleEmptyFileTable()
{
   const bool isEmpty = (0 == fFileTable->getNumRows());
   fFileTable->setVisible(!isEmpty);
   fEmptyView->setVisible(isEmpty);
}


void FileListView::ReloadFileTable(bool clearSelection/*=false*/)
{
   fFileTable->LoadFiles(fToolBar->SelectedSortFile().Key(), fToolBar->TagsToInclude(),
                         clearSelection);
   this->HandleEmptyFileTable();
}


void FileListView::RemoveSelectedFiles()
{
   fTaggingManager.RemoveFilesIn(fFileTable->SelectedFileKeys());
   this->ReloadFileTable(true);
}

//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="FileListView" componentName=""
                 parentClasses="public ContentView, public ChangeListener, public FileDragAndDropTarget"
                 constructorParams="" variableInitialisers="" snapPixels="8" snapActive="1"
                 snapShown="1" overlayOpacity="0.330" fixedSize="0" initialWidth="600"
                 initialHeight="400">
  <METHODS>
    <METHOD name="parentSizeChanged()"/>
    <METHOD name="filesDropped (const StringArray&amp; filenames, int mouseX, int mouseY)"/>
  </METHODS>
  <BACKGROUND backgroundColour="88111111"/>
  <GENERICCOMPONENT name="empty view" id="e6e4772fca9ab902" memberName="fEmptyView"
                    virtualName="" explicitFocusOrder="0" pos="0 0R 100% 110M" posRelativeX="c47e1d8c07185f70"
                    posRelativeY="c47e1d8c07185f70" posRelativeW="c47e1d8c07185f70"
                    class="AEmptyFileListView" params=""/>
  <GENERICCOMPONENT name="file table" id="97c2374c7d1fc05b" memberName="fFileTable"
                    virtualName="" explicitFocusOrder="0" pos="0 0R 100% 110M" posRelativeX="c47e1d8c07185f70"
                    posRelativeY="c47e1d8c07185f70" posRelativeW="c47e1d8c07185f70"
                    class="ADetailFileTable" params=""/>
  <TEXTBUTTON name="add files button" id="ac2b6a4200b0c525" memberName="fAddFilesButton"
              virtualName="" explicitFocusOrder="0" pos="0 35R 150 32" posRelativeX="c47e1d8c07185f70"
              bgColOff="a45c94" bgColOn="181f22" buttonText="Add Files" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <GENERICCOMPONENT name="tool bar" id="c47e1d8c07185f70" memberName="fToolBar" virtualName=""
                    explicitFocusOrder="0" pos="11 11 22M 50" class="AFileToolBar"
                    params=""/>
  <TEXTBUTTON name="remove button" id="bef8d9fe5973a5c0" memberName="fRemoveButton"
              virtualName="" explicitFocusOrder="0" pos="-10R 0Cc 150 32" posRelativeX="ac2b6a4200b0c525"
              posRelativeY="ac2b6a4200b0c525" bgColOff="a45c94" bgColOn="181f22"
              textCol="ffb4b8b5" buttonText="Remove" connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

