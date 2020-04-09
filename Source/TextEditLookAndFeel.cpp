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

#include "TextEditLookAndFeel.h"


void ATextEditLookAndFeel::drawTextEditorOutline(Graphics& g, int width, int height,
                                                 TextEditor& textEditor)
{
   // Same as V4 look and feel except for rounded ractangles.
   if (dynamic_cast<AlertWindow*>(textEditor.getParentComponent()) == nullptr)
   {
      if (textEditor.isEnabled())
      {
         if (textEditor.hasKeyboardFocus(true) && ! textEditor.isReadOnly())
         {
            g.setColour(textEditor.findColour(TextEditor::focusedOutlineColourId));
            g.drawRoundedRectangle(0, 0, width, height, fCornerSize, 1);
         }
         else
         {
            g.setColour(textEditor.findColour(TextEditor::outlineColourId));
            g.drawRoundedRectangle(0, 0, width, height, fCornerSize, 1);
         }
      }
   }
}
