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

#include <map>

#include "AppLookAndFeel.h"


/** Draw file list table headers */
class AHeaderLookAndFeel : public AAppLookAndFeel
{
public:
   AHeaderLookAndFeel();
   AHeaderLookAndFeel(float fontSize)
   : fFontSizePixels(fontSize)
   {}

   void drawTableHeaderColumn(Graphics&, TableHeaderComponent&,
                              const String& columnName, int columnId,
                              int width, int height,
                              bool isMouseOver, bool isMouseDown, int columnFlags) override;

   std::map<int, Justification::Flags> fJustifications;
   float fFontSizePixels = 17.0f;
};
