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

#include <memory>
#include <regex>
#include <sstream>

#include "DataSet.h"
#include "Paths.h"
#include "PaxArchive.h"
#include "RemoteTrainer.h"
#include "SecureSession.h"


const char* const ARemoteTrainer::kDataArchiveName = "Preproc";
const char* const ARemoteTrainer::kWeightsName = "weights.pb";
const char* const ARemoteTrainer::kLabelsName = "class_names.txt";

namespace
{
   using Channel = std::unique_ptr<ASecureChannel>;
   using Progress = ASecureSession::ProgressInfo;

   constexpr int uPercentComplete(const Progress& info) noexcept
   {
      return static_cast<int>(info.completed * 100ULL / info.total);
   }

   /**
    * Functor to monitor output fed to it line by line from which it extracts
    * the training progress information.
    */
   class TrainingProgress
   {
   public:
      TrainingProgress(int* progress, float phase, ARemoteTrainer::ResultCallback);
      void operator()(const std::string_view& line);

   private:
      ARemoteTrainer::ResultCallback fCallback;
      std::regex fProgressRegex;
      const int kProgressStart;
      const float kPhases;
      int* fProgress;

   };
}

ARemoteTrainer::ARemoteTrainer(const std::string& user, const std::string& host)
: fUserName(user)
, fHostName(host)
, fProgress(0)
{}

ARemoteTrainer::ARemoteTrainer(const std::string&& user, const std::string&& host)
: fUserName(std::move(user))
, fHostName(std::move(host))
, fProgress(0)
{}


int ARemoteTrainer::Train(const File& archive, const File& outDir,
                          ResultCallback resultCallback) noexcept(false)
{
   fErrors.clear();
   std::string msg = "--";
   try
   {
      fProgressPhases = 5.0f;
      fProgress = 0;
      this->ConnectTrainAndDownload(archive, outDir, resultCallback);
   }
   catch (ssh::SshException& ex)
   {
      fProgress = -1;
      msg = "SshException: " + ex.getError();
   }
   catch (const std::exception& ex)
   {
      fProgress = -1;
      msg = std::string("Exception: ") + ex.what();
   }
   catch (...)
   {
      fProgress = -1;
      msg = "Unknown exception";
   }

   resultCallback((msg + "\n").c_str(), fProgress);

   fErrors.push_back(std::move(msg));

   return 0;
}


File ARemoteTrainer::ArchiveDataSet(const File& directory, ResultCallback resultCallback) const
{
   // TODO: Add progress info to CreateArchive?

   File archive { ADataSet(directory, 0.8f).CreateArchive(kDataArchiveName) };

   fProgress += 100 / fProgressPhases;
   resultCallback("Data set archived.\n", fProgress);

   return archive;
}


File ARemoteTrainer::CheckForFailedTraining()
{
   auto found { ADataSet::WorkingDirectory()
                 .getChildFile(ADataSet::MakeArchiveName(kDataArchiveName)) };
   return found.exists() ? found : File();
}


void ARemoteTrainer::ConvertTrainingResults(ASecureSession& session,
                                            ResultCallback resultCallback) const
{
   resultCallback("Preparing sort file...", fProgress);

   auto channel { session.OpenChannel() };
   std::string result = channel->ResultOfCommand(this->ConversionScript());

   // The script will return nothing if it works. If the command fails
   // there may be an error message, or the channel may construct one.
   if (!result.empty())
   {
      resultCallback(result.c_str(), fProgress);
   }
   else
   {
      fProgress += 100 / fProgressPhases;
      resultCallback("done\n", fProgress);
   }
}


bool ARemoteTrainer::AskToTrustServer(const char* serverThumbprint)
{
   int answer =
    AlertWindow::showOkCancelBox(AlertWindow::WarningIcon,
     TRANS("Unknown Server"),
     TRANS("Do you want to trust the server with the following host key?\n\n") +
           serverThumbprint);
   return 1 == answer;
}


ASecureSession* ARemoteTrainer::Connect(ResultCallback resultCallback)
{
   resultCallback("Connecting...", fProgress);

   // libSSH does provide callbacks with connection progress information if we want it.

   auto sshConfigFilename = Paths::HomeDirectoryFile(".ssh/config");
   auto session = std::make_unique<ASecureSession>(fUserName, fHostName,
                                                   ASecureSession::kDefaultPort,
                                                   sshConfigFilename.getFullPathName().toStdString(),
                                                   &ARemoteTrainer::AskToTrustServer);
   fProgress += 100 / fProgressPhases;
   resultCallback("connected.\n", fProgress);
   return session.release();
}


void ARemoteTrainer::ConnectTrainAndDownload(const File& archive,
                                             const File& outDir,
                                             ResultCallback resultCallback) noexcept(false)
{
   auto session = std::unique_ptr<ASecureSession>(this->Connect(resultCallback));

   this->Upload(*session, archive, resultCallback);
   this->Train(*session, archive, resultCallback);
   this->ConvertTrainingResults(*session, resultCallback);
   this->RetrieveWeights(*session, outDir, resultCallback);
   this->RetrieveLabels(*session, outDir, resultCallback);
}


const std::vector<std::string>& ARemoteTrainer::Errors() const
{
   return fErrors;
}

void ARemoteTrainer::RetrieveLabels(ASecureSession &session, const File& dest,
                                    ResultCallback resultCallback) const
{
   const int progressStart = fProgress;
   resultCallback("\nDownloading tag names file", fProgress);

   auto result = std::string("\nDownload of ") + kLabelsName + " succeeded.\n";

   int readResult = session.ReadFile(kLabelsName, dest.getFullPathName().toStdString(),
    [progressStart, resultCallback, this](const Progress& info)
    {
       fProgress = progressStart + uPercentComplete(info) / fProgressPhases;
       resultCallback(".", fProgress);
    });

   if (0 != readResult)
   {
      throw std::runtime_error("There was an error downloading the tag names. Check the log.");
   }

   fProgress = 100;
   resultCallback(result.c_str(), fProgress);
}


void ARemoteTrainer::RetrieveWeights(ASecureSession &session, const File& dest,
                                     ResultCallback resultCallback) const
{
   const int progressStart = fProgress;
   resultCallback("Downloading sort file ", fProgress);

   auto result = std::string("\nDownload of ") + kWeightsName + " succeeded.\n";

   int readResult =
    session.ReadFile("./" + WeightsArchiveName().toStdString(), dest.getFullPathName().toStdString(),
     [progressStart, resultCallback, this](const Progress& info)
     {
        fProgress = progressStart + uPercentComplete(info) / fProgressPhases;
        resultCallback(".", fProgress);
     });

   if (0 != readResult)
   {
      throw std::runtime_error("There was an error downloading the sort file. Check the log.");
   }

   // Extract into the same location as the archive.
   APaxExtractor::ExtractAll(dest.getChildFile(WeightsArchiveName()), dest);

   resultCallback(result.c_str(), fProgress);
}


void ARemoteTrainer::Train(ASecureSession& session,
                                   const File& archive,
                                   ResultCallback resultCallback) const
{
   resultCallback("Training... ", fProgress);
   
   Channel channel(session.OpenChannel());

   bool success = channel->ResultOfCommand(this->TrainingScript(archive),
    TrainingProgress(&fProgress, fProgressPhases, resultCallback));

   if (success)
   {
      resultCallback("completed\n", fProgress);
   }
   else
   {
      resultCallback("failed\n", -1);
   }
}


std::string ARemoteTrainer::TrainingScript(const File& archive) const
{
   std::stringstream command;

   command
    << "dataDirName=" << kDataArchiveName << ";"
    << "archiveFilename=" << archive.getFileName().toStdString() << ";"
    << "weightsName=" << kWeightsName << ";"
    << "weightsArchiveName=" << WeightsArchiveName() << ";"
    ;

   command <<

    // Clean up previous training data.
    "rm -rf ./$dataDirName log.txt weights.hdf5 ./$weightsName ./$weightsArchiveName;"

    // Do the training.
    // TODO: Get epochs and value split from some configuration
    "tar xf ./$archiveFilename 2>&1;"
    "./panotti/train_network.py --epochs=20 --val=0.1 2>&1 | tee log.txt;"
    ;

   return command.str();
}


std::string ARemoteTrainer::ConversionScript() const
{
   std::stringstream command;

   command
    << "weightsName=" << kWeightsName << ";"
    << "weightsArchiveName=" << WeightsArchiveName() << ";"
    ;

    // Convert weights to protobuf format and archive the protobuff files.
   command <<
    "./panotti/utils/h5topb.py weights.hdf5 ./$weightsName 2>&1;"
    "tar czf ./$weightsArchiveName $weightsName;"
    ;

   return command.str();
}


String ARemoteTrainer::WeightsArchiveName()
{
   return APaxArchive::MakeArchiveName(kWeightsName);
}


void ARemoteTrainer::Upload(ASecureSession& session, const File& file,
                            ResultCallback resultCallback) const
{
   const int progressStart = fProgress;
   resultCallback("Uploading", fProgress);
   
   auto filename = file.getFullPathName().toStdString();
   auto result = std::string("\nUpload of ") + filename + " succeeded.\n";

   int writeResult =
    session.WriteFile(filename, file.getSize(), file.getFileName().toStdString(),
     [progressStart, resultCallback, this](const Progress& info)
     {
        fProgress = progressStart + uPercentComplete(info) / fProgressPhases;
        resultCallback(".", fProgress);
     });

   if (0 != writeResult)
   {
      throw std::runtime_error("Error uploading. Check the log.");
   }

   resultCallback(result.c_str(), fProgress);
}


namespace
{
   TrainingProgress::TrainingProgress(int* progress, float phases,
                                      ARemoteTrainer::ResultCallback callback)
   : fCallback{callback}
   , fProgressRegex{"Epoch (\\d+)/(\\d+)"} // e.g. Epoch 10/20
   , kProgressStart{*progress}
   , kPhases{phases}
   , fProgress{progress}
   {}

   void TrainingProgress::operator()(const std::string_view& line)
   {
      using namespace std;
      std::match_results<std::string_view::const_iterator> match;
      if (regex_search(line.begin(), line.end(), match, fProgressRegex))
      {
         *fProgress = kProgressStart + 100 *
          (stof(match.str(1)) / stof(match.str(2)) / kPhases);
      }
      assert(0 <= *fProgress & 100 >= *fProgress);

      fCallback(string(line.data(), line.size()).c_str(), *fProgress);
   }
}
