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

#include "../JuceLibraryCode/JuceHeader.h"

#include <memory>
#include <string>

/**
 * Singleton class for maintaining app wide values.
 *
 * One instance of a SharedResourcePointer<ASharedSettings> must
 * be created and held by some object for as long as the settings should be valid
 * for ASettings::Instance to return a valid pointer.
 */
class ASettings
{
public:
   
   static ASettings& Instance();
   ~ASettings();
   
   // REAL SETTINGS
   
   std::string Username() const noexcept;
   void SetUsername(const std::string& username);
   
   std::string Server() const noexcept;
   void SetServer(const std::string& server);

   // TEMP/DEBUG SETTINGS
   
   const std::string& DebugRootPath() const;
   void SetDebugRootPath(const std::string& path);

   bool SaveSpectrogramPng() const;
   void SetSaveSpectrogramPng(bool doSave);
   
   bool SaveTensorPng() const;
   void SetSaveTensorPng(bool doSave);

private:

   friend struct ASharedSettings;
   
   ASettings();
   ASettings(const ASettings&) = delete;

   void Read();
   void Write();
   
   std::string fDebugRootPath;
   std::string fServer;
   std::string fUsername;
   bool fSaveSpectrogramPng;
   bool fSaveTensorPng;

   ApplicationProperties fAppProperties;
};

struct ASharedSettings
{
   ASharedSettings() : instance() {}
   ASettings instance;
};
