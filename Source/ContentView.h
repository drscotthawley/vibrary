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

#include "MainView.h"

/**
 * A base class for views that will be displayed in MainView's content area.
 */
class ContentView : public Component
{
public:
   void SetMainView(MainView* mainView) { fMainView = mainView; }

   /** Called on the current view before displaying the new content view. */
   virtual void LeavingView() {}
   
protected:
   MainView* fMainView;
};
