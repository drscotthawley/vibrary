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

/**
 * Draws outline frame as rounded rectangles.
 */
class ATextEditLookAndFeel : public AAppLookAndFeel
{
public:
   ATextEditLookAndFeel()
   {}
   
   ATextEditLookAndFeel(float cornerSize, float fontSize)
   : fCornerSize(cornerSize), fFontSizePixels(fontSize)
   {}
   
   void drawTextEditorOutline(Graphics& g, int width, int height, TextEditor& textEditor) override;

   float fCornerSize = 10.0f;
   float fFontSizePixels = 22.0f;
};

