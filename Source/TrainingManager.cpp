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
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <thread>

#include "Colorizers.h"
#include "DataSetBuilder.h"
#include "Paths.h"
#include "Persistence.h"
#include "RemoteTrainer.h"
#include "Settings.h"
#include "TrainingManager.h"
#include "Utils.h"


namespace
{
   TagLabels MakeTagLabels(const File& labelsFile);
}


ATrainingManager::ATrainingManager()
: fSortFileName(kDefaultSortFileName)
{}


void ATrainingManager::SetSortFileName(const String& name)
{
   fSortFileName = name;
}


void ATrainingManager::StoreTrainingResults(const File& trainingDir)
{
   if (fSortFileName.isEmpty())
   {
      fSortFileName = kDefaultSortFileName;
   }
   ATrainingManager::StoreTrainingResults(trainingDir, fSortFileName);
}


void ATrainingManager::LaunchTraining(const ExampleData& examples,
                                      ProgressFunc progressFunc)
{
   std::thread([examples, progressFunc, this]()
    {
       // In all cases we want to delete the training dir after this process is
       // complete. So using the custom deleter of unique_ptr
       auto trainingDir {
        std::unique_ptr<File, ADirRemover> {
         new File { Paths::MakeDir(Paths::DataDirectory(), fSortFileName) },
         { TRANS("Failed to delete the training data directory") }
       }};

      float completed = 0.0f;

      auto builder { ADataSetBuilder { *trainingDir } };

      const size_t fileCount =
       std::accumulate(examples.begin(), examples.end(), 0UL,
        [](size_t total, const auto& tagFilesPair)
        {
           return total + tagFilesPair.second.size();
        });

      // Half progress given to dataset creation, half to remote training.
      const float kProgressPerFile = 50.0f / fileCount;

      std::for_each(examples.begin(), examples.end(),
       [builder, &completed, kProgressPerFile, progressFunc](const auto& tagFilesPair)
       {
          const auto& tagName = tagFilesPair.first;
          const auto& files = tagFilesPair.second;

          std::for_each(files.begin(), files.end(),
           [builder, &completed, kProgressPerFile, progressFunc, tagName](const auto& file)
           {
              completed += kProgressPerFile;
              try
              {
                 progressFunc(completed,
                              (TRANS("Adding ") + file.getFileName()).toStdString().c_str(),
                              Contrasty(builder.AddFile(file, tagName)));
              }
              catch (std::runtime_error& err)
              {
                 std::cerr << err.what() << "\n";
                 progressFunc(-1, // signal the error
                              (TRANS("Ignoring ") + file.getFileName()).toStdString().c_str(),
                              Image());
              }
           });
       });

      // If there is an error somewhere in the training process, we don't want
      // to delete the archive. So we won't use a unique_ptr and custom deleter
      // as we did with the trainingDir.
      const auto archive {
       builder.MakeDataSetArchive(ARemoteTrainer::kDataArchiveName,
                                  kTrainingPartitionPercent)
      };

      auto trainer { ARemoteTrainer(ASettings::Instance().Username(),
                                    ASettings::Instance().Server()) };

      trainer.Train(archive, *trainingDir,
       [&completed, progressFunc](const char* result, int percentComplete)
       {
          completed = 49.0f + percentComplete * 0.5f;
          progressFunc(completed, result, juce::Image());
       });

      // When remote trainer is done, we expect completed to be 99 if all steps
      // were successful.
      if (completed > 98.0f)
      {
         // Leave the archive in case of failure because we can then try
         // the training again if we find an archive sitting around.
         archive.deleteFile();
         this->StoreTrainingResults(*trainingDir);

         progressFunc(100.0f, "Completed", juce::Image());
      }
      else if (trainer.HasErrors())
      {
         std::stringstream msgs;
         std::copy(trainer.Errors().begin(), trainer.Errors().end(),
                   std::ostream_iterator<std::string>(msgs, "\n"));

         progressFunc(-1.0f, msgs.str().c_str(), juce::Image());
      }

   }).detach(); //$$$ GSL says don't use detach this way
}




// MARK: - Static API

bool ATrainingManager::HandleIfFailedTraining()
{
   int completed = 0;
   const File archive { ARemoteTrainer::CheckForFailedTraining() };
   if (archive.existsAsFile())
   {
      bool cleanUp = false;

      const int answer =
       AlertWindow::showYesNoCancelBox(AlertWindow::QuestionIcon,
                                       TRANS("Recover Training?"),
                                       TRANS("It appears that a previous training was not completed. Do you want "
                                             "to try again?"),
                                       TRANS("Yes"), TRANS("No"), TRANS("Remove It"));

      enum { kRemoveIt, kYes, kNo };

      switch (answer)
      {
         case kYes:
         completed = ATrainingManager::TryToResumeTraining(archive);
         cleanUp = completed >= 100;
         break;

         case kRemoveIt:
         cleanUp = true;
         break;
      }

      if (cleanUp && !archive.deleteFile())
      {
         AlertWindow::showMessageBox(AlertWindow::InfoIcon,
                                     TRANS("Delete Failed"),
                                     TRANS("The archive was not deleted."));
      }
   }
   return completed >= 100;
}


void ATrainingManager::StoreTrainingResults(const File& trainingDir,
                                            String& sortFileName)
{
   jassert(!sortFileName.isEmpty());

   const auto weightFile = trainingDir.getChildFile(ARemoteTrainer::kWeightsName);
   const auto labelsFile = trainingDir.getChildFile(ARemoteTrainer::kLabelsName);
   const auto sortDir = Paths::MakeUniqueDir(Paths::WeightsDirectory(),
                                             sortFileName);
   const auto uniqueName = sortDir.getFileName();
   const auto movedWeightFile = sortDir.getChildFile(uniqueName + "Weights.pb");
   const auto movedLabelsFile = sortDir.getChildFile(uniqueName + "Labels.txt");

   bool moveSuccess = true;

   if (labelsFile.existsAsFile())
   {
      moveSuccess = labelsFile.moveFileTo(movedLabelsFile);
   }
   else
   {
      std::cerr << "Weights archive did not include labels. "
       "Expected at " << labelsFile.getFullPathName() << "\n";
   }

   // Handle both self-contained models and models in a directory of files.
   if (weightFile.existsAsFile())
   {
      moveSuccess = weightFile.moveFileTo(movedWeightFile) && moveSuccess;
   }
   else
   {
      moveSuccess = weightFile.copyDirectoryTo(movedWeightFile) && moveSuccess;
      weightFile.deleteRecursively();
   }

   if (!moveSuccess)
   {
      throw std::runtime_error("Failed to move sort file data into place");
   }

   Persist::AddSortFile(uniqueName.toStdString(),
                        movedWeightFile.getFullPathName().toStdString(),
                        MakeTagLabels(movedLabelsFile));
}


int ATrainingManager::TryToResumeTraining(const File& archive)
{
   int completed = 0;
   std::string lastMsg;
   const auto trainingDir = Paths::DataDirectory();

   ARemoteTrainer(ASettings::Instance().Username(), ASettings::Instance().Server())
   (archive, trainingDir,
    [&completed, &lastMsg](const char* result, int percentComplete)
    {
       completed = percentComplete;
       if (completed < 0)
       {
          lastMsg = result;
       }
       std::cerr << result;
    });

   if (completed < 0)
   {
      AlertWindow::showMessageBox(AlertWindow::WarningIcon,
                                  TRANS("Training Failed"), lastMsg);
   }
   else if (completed == 100)
   {
      String unknown = "Recovered";
      ATrainingManager::StoreTrainingResults(trainingDir, unknown);

      AlertWindow::showMessageBox(AlertWindow::InfoIcon,
                                  TRANS("Success"),
                                  TRANS("The training has succeeded.")); // add new sort file name?
   }
   else if (completed < 100)
   {
      AlertWindow::showMessageBox(AlertWindow::InfoIcon,
                                  TRANS("Training Failed"),
                                  TRANS("The training has not completed, and no error was reported."));
   }

   return completed;
}

namespace
{
   TagLabels MakeTagLabels(const File& labelFile)
   {
      auto tagLabels = TagLabels();
      int classId = 0;
      for (const auto& label : Persist::ReadStrings(labelFile.getFullPathName().toStdString()))
      {
         tagLabels[classId++] = label;
      }
      return tagLabels;
   }
}
