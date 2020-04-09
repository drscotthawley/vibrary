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

#include "../JuceLibraryCode/JuceHeader.h"

#include "Paths.h"
#include "Settings.h"


ASettings& ASettings::Instance()
{
   return SharedResourcePointer<ASharedSettings>()->instance;
}


ASettings::ASettings()
: fDebugRootPath("/Users/jason/Desktop/")// TODO: Useful default for other users
, fServer("incubator.artlogicdev.net")
, fUsername("sorting")
, fSaveSpectrogramPng(false)
, fSaveTensorPng(false)
{
   auto options = PropertiesFile::Options();

   // This object is created during static init or very early in the app life,
   // and the information Paths relies on may not be valid yet.
   options.applicationName = "Sorting";//Paths::ApplicationName();
   options.commonToAllUsers = false;
   options.doNotSave = false;
   options.filenameSuffix = ".settings";
   options.osxLibrarySubFolder = "Application Support";
   options.folderName = "Sorting";
   options.storageFormat = PropertiesFile::storeAsXML;
   options.millisecondsBeforeSaving = 300;

   fAppProperties.setStorageParameters(options);

   this->Read();
}


ASettings::~ASettings()
{}


std::string ASettings::Username() const noexcept
{
   return fUsername;
}

void ASettings::SetUsername(const std::string& username)
{
   if (username != fUsername)
   {
      fUsername = username;
      this->Write();
   }
}

std::string ASettings::Server() const noexcept
{
   return fServer;
}

void ASettings::SetServer(const std::string& server)
{
   if (server != fServer)
   {
      fServer = server;
      this->Write();
   }
}

const std::string& ASettings::DebugRootPath() const
{
   return fDebugRootPath;
}


bool ASettings::SaveSpectrogramPng() const
{
   return fSaveSpectrogramPng;
}


bool ASettings::SaveTensorPng() const
{
   return fSaveTensorPng;
}


void ASettings::SetDebugRootPath(const std::string& path)
{
   fDebugRootPath = path;
   this->Write();
}


void ASettings::SetSaveSpectrogramPng(bool doSave)
{
   fSaveSpectrogramPng = doSave;
   this->Write();
}


void ASettings::SetSaveTensorPng(bool doSave)
{
   fSaveTensorPng = doSave;
   this->Write();
}

// MARK: - Persistence

namespace
{
   const char* const kServerKey = "server";
   const char* const kUsernameKey = "username";

   const char* const kDebugRootPathKey = "debugRootPath";
   const char* const kSaveSpectrogramPngKey = "saveSpectrogramPng";
   const char* const kSaveTensorPngKey = "saveTensorPng";
}

void ASettings::Read()
{
   auto file = fAppProperties.getUserSettings();

   fServer = file->getValue(kServerKey, fServer.c_str()).toStdString();
   fUsername = file->getValue(kUsernameKey, fUsername.c_str()).toStdString();

   fDebugRootPath = file->getValue(kDebugRootPathKey, fDebugRootPath.c_str()).toStdString();
   fSaveSpectrogramPng = file->getBoolValue(kSaveSpectrogramPngKey, fSaveSpectrogramPng);
   fSaveTensorPng = file->getBoolValue(kSaveTensorPngKey, fSaveTensorPng);
}


void ASettings::Write()
{
   auto file = fAppProperties.getUserSettings();

   file->setValue(kServerKey, this->Server().c_str());
   file->setValue(kUsernameKey, this->Username().c_str());

   file->setValue(kDebugRootPathKey, this->DebugRootPath().c_str());
   file->setValue(kSaveSpectrogramPngKey, this->SaveSpectrogramPng());
   file->setValue(kSaveTensorPngKey, this->SaveTensorPng());
}
