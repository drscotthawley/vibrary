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

class AAppLookAndFeel : public LookAndFeel_V4
{
public:
   /** Set up colors and default font. */
   AAppLookAndFeel();
   
   /** If not loaded, load Montserrat typeface and store in sMontserrat. */
   static Typeface::Ptr GetMontserratFace();
   
   /** delete static objects */
   static void CleanUp();
   
   static const Colour kLightGray;
   static const Colour kMediumGray;
   static const Colour kDarkGray;
   static const Colour kGreenProgress;
   static const Colour kPurpleNav;
   
   
private:
   static Typeface::Ptr sMontserrat;
};

