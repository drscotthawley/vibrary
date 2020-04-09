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
#include <string>

#include "../JuceLibraryCode/JuceHeader.h"

class ASecureSession;

// I'm using a class here when a function may do to facilitate future
// concurrency. No, really.

class ARemoteTrainer
{
public:
   /**
    * The callback receives a status message and the percent of the training
    * that has been completed. A negative value indicates the training failed.
    */
   using ResultCallback = std::function<void(const char*, int32_t)>;

   ARemoteTrainer(const std::string& username, const std::string& hostname);
   ARemoteTrainer(const std::string&& username, const std::string&& hostname);

   /**
    * Train with a prepared data set, archived in dataSetArchive. Place results
    * in outDir.
    */
    int Train(const File& dataSetArchive, const File& directory, ResultCallback) noexcept(false);

   // Functor interface for easy use with threads, etc.

   int operator()(const File& dataSetArchive, const File& outDir,
                  ResultCallback callback) noexcept(false)
   {
      return this->Train(dataSetArchive, outDir, callback);
   }

   constexpr bool HasErrors() const { return !fErrors.empty(); }
   const std::vector<std::string>& Errors() const;

   /** If an archive is found, return that path. Otherwise returns empty File object. */
   static File CheckForFailedTraining();

   /** The name the server expects for the archive file. */
   static const char* const kDataArchiveName;
   /** The name the server gives its weights file output. */
   static const char* const kWeightsName;
   /** The name the server gives its labels file output. */
   static const char* const kLabelsName;

   static String WeightsArchiveName();

private:

   /** Perform all the training steps (except ArchiveDataSet) */
   void ConnectTrainAndDownload(const File& archive, const File& outDir,
                                ResultCallback) noexcept(false);

      // Training steps.

   File ArchiveDataSet(const File& directory, ResultCallback) const;
   ASecureSession* Connect(ResultCallback);
   void Upload(ASecureSession& session, const File& file, ResultCallback) const;
   void Train(ASecureSession& session, const File& archive, ResultCallback) const;
   void ConvertTrainingResults(ASecureSession& session, ResultCallback) const;
   void RetrieveWeights(ASecureSession& session, const File& destination, ResultCallback) const;
   void RetrieveLabels(ASecureSession& session, const File& destination, ResultCallback) const;

   static bool AskToTrustServer(const char* serverThumbprint);

   std::string TrainingScript(const File& archive) const;
   std::string ConversionScript() const;

   std::vector<std::string> fErrors;
   std::string fUserName;
   std::string fHostName;
   mutable int fProgress;
   int fProgressPhases;
};
