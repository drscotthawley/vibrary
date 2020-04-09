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

/**
 * \file A central place to keep commands so we can make sure they do not overlap.
 * These are expected to be posted to components using postCommandMessage(int).
 */

/** Commands related to AddTagsView. */
namespace AddTags
{
   enum Command
   {
      kCancel,
      kSave,
      /** This command is posted when user clicks special AddTag button. */
      kAddTag,

      /**
       * kSetStrength, hi word is the command, low word is the number of files
       */
      kUpdateStrength = 0x00010000
   };
}


namespace Overlay
{
   /** Commands posted to owner from Overlays */
   enum TxCommands { kDismissed = 0x0010, kAccepted };

   /** Commands handled by Overlays */
   enum RxCommands { kExit = 0x0012 };
}

/** Command related to file views. */
namespace Files
{
   enum RxCommands
   {
      kTaggingError = 0x0020,
      kReloadTable,
      kPreviewToggle // toggle previewing of a selected file
   };
}


namespace Training
{
   enum TxCommand { kTrainingDone = 0x0040, kSortFileAdded };
};
