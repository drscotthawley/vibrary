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

#include <memory>
#include <string>

#include "../JuceLibraryCode/JuceHeader.h"

class ANetFeeder;
class ASpectrogram;

/**
 * Makes predictions of an audio file's category given
 * weights and  for the graph.
 */
class ATagger
{
public:
   /**
    * \param graphName The name of the resource which holds the weights of the
    *                  graph. It is modified to get the name of the resource file
    *                   with tag names replacing Weights.pb with Labels.txt.
    */
   ATagger(const std::string& graphName);
   // TODO: Name of sortfile, load weigths and tag names from DB.
   ~ATagger();
   
   /**
    * Predict the class for the given file,
    * i.e. assign it a tag with the graph this tagger is using.
    * \param file An audio file that can be converted to a spectrogram.
    * \return The class id of predicted tag of the file or -1 if there was an error. */
   int TagClassIdFor(const File& file) const noexcept(false);

   /**
    * Predict the class for the given file,
    * i.e. assign it a tag with the graph this tagger is using.
    * \return The class id of predicted tag of the file or -1 if there was an error. */
   int TagClassIdFor(const ASpectrogram& spectrogram) const noexcept(false);
   
   /**
    * \returns true if the tagger can handle this file type, false otherwise.
    */
   static bool CanTagFile(const File& file);
   
   static StringArray AudioFileTypes() noexcept
   {
      // Many more are possible, see juce::AudioFormatManager.
      return StringArray(".wav",
                         ".aif", ".aiff", ".aifc", ".caf",
                         ".mp3", ".m4r", ".mpa", ".m4a", ".mp4",
                         ".ogg", ".flac");

   }

private:

   std::string LabelNetResults() const;
   int PredictedClass() const noexcept(false);
   
   void MakeNetFeeder(const std::string& graphName);
   
   std::unique_ptr<ANetFeeder> fNetFeeder;
};
