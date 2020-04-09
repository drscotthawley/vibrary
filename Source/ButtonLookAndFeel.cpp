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

#include "ButtonLookAndFeel.h"

const Colour AButtonLookAndFeel::kGradientColor[] =
{
   Colour(0x91, 0x93, 0x94), // disabled 1
   Colour(0x52, 0x52, 0x52), // disabled 2
   Colour(0x00, 0xa6, 0xff), // enabled 1
   Colour(0xa3, 0x19, 0xb0), // enabled 2
};


AButtonLookAndFeel::AButtonLookAndFeel()
{
   auto colourScheme = this->getCurrentColourScheme();
   // This color isn't what is in the spec document but it is what is used
   // in the UXPin design.
   colourScheme.setUIColour(ColourScheme::defaultText, Colour(0xDE, 0xE1, 0xDF));
   this->setColourScheme(colourScheme);
}


// MARK: - Overrides

Font AButtonLookAndFeel::getTextButtonFont(TextButton&, int buttonHeight)
{
   return Font(fFontSizePixels);
}

void AButtonLookAndFeel::drawButtonBackground(Graphics& g, Button& button, const Colour& bg,
                                              bool shouldHilite, bool shouldDrawAsDown)
{   
   auto buttonArea = button.getLocalBounds();
   
   // Linear gradient tilted 20 degrees.
   const float kGradRise = ::tan(45.0f * MathConstants<float>::pi / 360.0f);
   
   const auto x1 = 0.0f;
   const auto y1 = 0.0f;
   const auto gradWidth = buttonArea.getWidth();
   const auto x2 = x1 + gradWidth;
   const auto y2 = gradWidth * kGradRise;
   
   // Map into color table based on enabled status.
   const int e = (button.isEnabled() ? 2 : 0);

   g.setGradientFill(ColourGradient(kGradientColor[0 + e], x1, y1,
                                    kGradientColor[1 + e], x2, y2,
                                    false));
   g.fillRoundedRectangle(buttonArea.getX(), buttonArea.getY(),
                          buttonArea.getWidth(), buttonArea.getHeight(), fCornerSize);
   
  // Test gradient angle
  // g.setColour(Colours::red);
  // g.drawLine(x1, y1, x2, y2);
}

