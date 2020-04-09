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

#include "AppLookAndFeel.h"


/** Draw blue->purple gradient, rounded corner buttons */
class AButtonLookAndFeel : public AAppLookAndFeel
{
public:
   AButtonLookAndFeel();
   AButtonLookAndFeel(float cornerSize, float fontSize)
   : fCornerSize(cornerSize), fFontSizePixels(fontSize)
   {}
   
   void drawButtonBackground(Graphics& g, Button& button, const Colour& bg,
                             bool shouldHilite, bool shouldDrawAsDown) override;
   
   // Enforce our desired font size
   Font getTextButtonFont (TextButton&, int buttonHeight) override;
   
   float fCornerSize = 22.0f;
   float fFontSizePixels = 22.0f;
   
   static const Colour kGradientColor[4];
};
