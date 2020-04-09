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

#include "HeaderLookAndFeel.h"

AHeaderLookAndFeel::AHeaderLookAndFeel()
: fFontSizePixels(15.0)
{}


void AHeaderLookAndFeel::drawTableHeaderColumn(Graphics& g, TableHeaderComponent& header,
                                               const String& columnName, int columnId,
                                               int width, int height,
                                               bool isMouseOver, bool isMouseDown, int columnFlags)
{
   // Mostly copied from LookAndFeel_V2

   auto highlightColour = header.findColour(TableHeaderComponent::highlightColourId);

   if (isMouseDown)
   {
      g.fillAll(highlightColour);
   }
   else if (isMouseOver)
   {
      g.fillAll(highlightColour.withMultipliedAlpha (0.625f));
   }

   Rectangle<int> area(width, height);
   area.reduce(4, 0);

   if ((columnFlags & (TableHeaderComponent::sortedForwards | TableHeaderComponent::sortedBackwards)) != 0)
   {
      const auto arrowHeight = std::min(height, this->getDefaultMenuBarHeight());
      auto arrowArea = area;
      arrowArea.setHeight(arrowHeight);
      arrowArea.setY((height - arrowHeight) / 2);

      Path sortArrow;
      sortArrow.addTriangle(0.0f, 0.0f,
                            0.5f, (columnFlags & TableHeaderComponent::sortedForwards) != 0 ? -0.8f : 0.8f,
                            1.0f, 0.0f);

      g.setColour(Colour(0x99000000));
      g.fillPath(sortArrow,
                 sortArrow.getTransformToScaleToFit(arrowArea.removeFromRight(arrowHeight / 2).reduced(2).toFloat(),
                                                    true));
   }

   g.setColour(header.findColour(TableHeaderComponent::textColourId));
   auto font = Font { GetMontserratFace() };
   font.setHeight(fFontSizePixels);
   g.setFont(font);

   auto justification = Justification::centredLeft;
   auto justIt = fJustifications.find(columnId);
   if (justIt != fJustifications.end())
   {
      justification = justIt->second;
   }

   g.drawFittedText(columnName, area.reduced(10, 0), justification, 1);
}
