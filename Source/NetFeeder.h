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
#include <vector>

#include "tensorflow/core/public/session.h"

#include "Graph.h"

class ASpectrogram;


/**
 * Loads a TensorFlow graph and feeds it input.
 */
class ANetFeeder
{
public:
   /** Read graph, create fSession. Can throw tf::Status. */
   ANetFeeder(const std::string& graphFilename, const std::string& inputLayerName,
              const std::string& outputLayerName, const std::vector<std::string>& labels);
   
   tensorflow::Status FeedForward(const ASpectrogram& spectrogram);
   
   /** Run the contents of the image file on the graph. Saves results in fOutputs. */
   tensorflow::Status FeedForward(const std::string& imageFilename);
   
   /**
    * This actually runs the graph with the given input. Other FeedForward
    * methods call this.
    */
   tensorflow::Status FeedForward(const tensorflow::Tensor& input_tensor);
 
   /**
    * \return The labels of the top K highest-scoring values from the
    * output of the last FeedForward run.
    */
   tensorflow::Status LabeledTopK(int k, std::vector<std::string>& labels) const;
   
   /**
    * \return The indices of the top K highest-scoring values from the
    * output of the last FeedForward run.
    */
   tensorflow::Status TopK(int k, std::vector<int>& indices) const;
   
private:
   using Tensors = std::vector<tensorflow::Tensor>;
   using SessionPtr = std::unique_ptr<tensorflow::Session>;
   
   std::pair<int64_t, int64_t> GraphInputsShape();

   // !!! TEMP
   void ModBatchNormNodes();
   
   std::string fInputLayerName;
   std::string fOutputLayerName;
   std::vector<std::string> fLabels;

   SavedModel fSavedModel;
   Graph fGraph;
   Tensors fOutputs; //!< Result of last FeedForward.
};

