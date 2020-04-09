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

#include "../JuceLibraryCode/JuceHeader.h"


/**
 * Play back an audio file.
 * I expect this to be used as a singleton.
 */
class APlayer : public ChangeBroadcaster
{
public:

   APlayer();
   ~APlayer();

   void SetAudioFile(const File& audioFile);

   void Prepare(int numSample, double rate);
   void Play(bool sendChange = false);
   bool IsPlaying() const;
   bool HasFinished() const;
   void GetNextBlock(const AudioSourceChannelInfo&);
   void Stop(bool sendChange = false);
   void ReleaseResources();

   void SeekToStart();

private:
   AudioFormatManager fFormatMgr;
   AudioTransportSource fTransport;
   std::unique_ptr<AudioFormatReaderSource> fSource;
};
