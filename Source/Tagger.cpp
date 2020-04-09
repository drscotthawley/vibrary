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

#include <regex>

#include "NetFeeder.h"
#include "Paths.h"
#include "Persistence.h"
#include "Spectrogram.h"
#include "Tagger.h"


namespace tf = tensorflow;

namespace
{
   constexpr int kPredictCount = 1;
}

ATagger::ATagger(const std::string& graphName)
{
   this->MakeNetFeeder(graphName);
}

ATagger::~ATagger()
{}


bool ATagger::CanTagFile(const File& file)
{
   return ATagger::AudioFileTypes().contains(file.getFileExtension().toLowerCase());
}


std::string ATagger::LabelNetResults() const
{
   std::vector<std::string> labels(kPredictCount);
   tf::Status status = fNetFeeder->LabeledTopK(kPredictCount, labels);
   if (status.ok())
   {
      return labels[0];
   }
   
   LOG(ERROR) << "Reading NN result failed: " << status;
   return "Error: " + status.ToString();
}


void ATagger::MakeNetFeeder(const std::string& graphFilename)
{
   //$$$: Can we get these from the graph?
   constexpr const char* inputLayerName = "Input_input";
   constexpr const char* outputLayerName = "Output/Softmax";
   
   // Scott's original graph had these layer names
   //constexpr const char* const inputLayerName = "conv2d_1_input";
   //constexpr const char* const outputLayerName = "activation_6/Softmax";
   
   // TODO: Get weights and tags from database.

   static auto sWeightsRegex = std::regex("Weights\\.pb");
   auto labelsFilename = std::regex_replace(graphFilename, sWeightsRegex, "Labels.txt");

   fNetFeeder = std::make_unique<ANetFeeder>(graphFilename,
                                             inputLayerName, outputLayerName,
                                             Persist::ReadStrings(labelsFilename));
}


int ATagger::TagClassIdFor(const File& file) const
{
   int predictedClass = -1;
   if (ATagger::CanTagFile(file))
   {
      predictedClass = this->TagClassIdFor(ASpectrogram(file.getFullPathName().toStdString()));
   }
   else
   {
      auto msg = std::string("Cannot handle file type: ")
       + file.getFileExtension().toStdString();
      LOG(ERROR) << msg << "\n";
      throw std::runtime_error(msg);
   }
   return predictedClass;
}


int ATagger::TagClassIdFor(const ASpectrogram& spectrogram) const
{
   int predictedClass = -1;
   const auto status = fNetFeeder->FeedForward(spectrogram);
   if (status.ok())
   {
      predictedClass = this->PredictedClass();
   }
   else
   {
      LOG(ERROR) << "Failed to process spectrogram input: " << status;
   }
   return predictedClass;
}


int ATagger::PredictedClass() const
{
   std::vector<int> prediction;
   tf::Status status = fNetFeeder->TopK(kPredictCount, prediction);
   if (status.ok())
   {
      return prediction[0];
   }
   else
   {
      std::string msg = TRANS("Failed to tag file\n").toStdString();
      msg += status.error_message();
      throw std::runtime_error(msg);
   }
   return -1;
}

