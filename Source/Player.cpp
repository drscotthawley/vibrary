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

#include <stdexcept>

#include "Player.h"


APlayer::APlayer()
{
   // Can we share this between ASpectrogramMaker and here?
   fFormatMgr.registerBasicFormats();
}


APlayer::~APlayer()
{
   fTransport.setSource(nullptr);
}


void APlayer::GetNextBlock(const AudioSourceChannelInfo& bufferInfo)
{
   if (fSource)
   {
      fTransport.getNextAudioBlock(bufferInfo);
   }
   else
   {
      bufferInfo.clearActiveBufferRegion();
   }
}


bool APlayer::HasFinished() const
{
   return fTransport.hasStreamFinished();
}


bool APlayer::IsPlaying() const
{
   return fTransport.isPlaying();
}


void APlayer::Play(bool sendChange/*=false*/)
{
   if (this->HasFinished())
   {
      this->SeekToStart();
   }
   fTransport.start();

   if (sendChange)
   {
      this->sendChangeMessage();
   }
}


void APlayer::Prepare(int numSamples, double sampleRate)
{
   fTransport.prepareToPlay(numSamples, sampleRate);
}


void APlayer::ReleaseResources()
{
   fTransport.releaseResources();
}


void APlayer::SeekToStart()
{
   fTransport.setPosition(0.0);
}


void APlayer::SetAudioFile(const File& audioFile)
{
   this->Stop();

   auto reader = fFormatMgr.createReaderFor(audioFile);
   if (!reader)
   {
      throw std::runtime_error("No audio format reader for file type.");
   }
   // make sure the current source exists for fTransport to deal with in setSource.
   auto newSource = std::make_unique<AudioFormatReaderSource>(reader, true);
   fTransport.setSource(newSource.get());
   fSource.reset(newSource.release());
}


void APlayer::Stop(bool sendChange/*=false*/)
{
   fTransport.stop();
   if (sendChange)
   {
      this->sendChangeMessage();
   }
}
