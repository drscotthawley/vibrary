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

#pragma once

#include <functional>

#include "../JuceLibraryCode/JuceHeader.h"


class ASpectrogram;


/**
 * Manage the work of building the example data set, training on the server,
 * handling training artifacts.
 */
class ATrainingManager
{
public:

   constexpr static const char* const kDefaultSortFileName = "Unnamed";

   /** The amount of the examples used as training data versus testing data. */
   constexpr static float kTrainingPartitionPercent = 0.8f;

   /** Map tag name to example files. */
   using ExampleData = std::map<String, Array<File>>;

   /**
    * The first arg is the completed percentage (0...100). If < 0, 2nd arg is an error message
    * Second arg is a text message indicating progress of the training.
    * Third argument is the last processed spectragram's image.
    * Either second arg or third arg is non-empty. (E.g.) If 2nd is nullptr, 3rd is valid.
    */
   using ProgressFunc = std::function<void(int, const char*, const juce::Image&)>;


   ATrainingManager();

   void SetSortFileName(const String& name);


   /**
    * Starts a thread to handle training with the given examples. Returns immediately
    * \param ProgressFunc is called with each spectrogram created, then with each
    */
   void LaunchTraining(const ExampleData& examples, ProgressFunc);

   /* Uses fSortfileName as context, asks user if undefined */
   void StoreTrainingResults(const File& trainingDir);
   //TODO: Retrain(ASortFile);
   //TODO: StoreRetrainingResults()

   constexpr const StringArray& Messages() const { return fMessages; }

   /**
    * See if atrifacts of a training are present, and ask user if the training
    * should be attempted again. If confirmed, attempt to train. Also gives
    * the user the option to clean up the artifacts instead
    * .
    *
    * \returns True if training was completed, false if user declined or training failed.
    */
   static bool HandleIfFailedTraining();

   /**
    * \returns Percent of training completed, 100 means success, less than
    *          that is a failure.
    */
   static int TryToResumeTraining(const File& archive);

   /**
    * If you already have a trained model this can be used to put it into the
    * database and data directory.
    * \param trainingDir The directory containing the model and label files.
    * \param sortFileName The root of the name for the sortfile. If that name is
    *                     already useda number is appended to it. On exit, it
    *                     contains the actual name used.
    */
   static void StoreTrainingResults(const File& trainingDir, String& sortFileName);


private:

   String fSortFileName;
   StringArray fMessages;
};
