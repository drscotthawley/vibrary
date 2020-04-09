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
#include "TagButton.h"

struct ASharedTagButtonLookAndFeel
{
   ASharedTagButtonLookAndFeel()
   : instance(ATagButton::kCornerSize, ATagButton::kTextSize)
   {}
   AButtonLookAndFeel instance;
};


ATagButton::ATagButton(const ATag& tag)
: TextButton(tag.Name())
, fTag(tag)
, fShowClose(false)
{
   this->setLookAndFeel(&fLookAndFeel->instance);
   this->setSize(kWidth, kHeight);
   this->setColour(TextButton::ColourIds::textColourOnId, Colours::white);
   this->setColour(TextButton::ColourIds::textColourOffId, Colours::white);
}


ATagButton::~ATagButton()
{
   this->setLookAndFeel(nullptr);
}


Rectangle<int> ATagButton::CloseBounds() const
{
   constexpr auto kWidth = 20;
   constexpr auto kHeight = 20;
   const auto x = this->getWidth() - kWidth - 5;
   const auto y = (this->getHeight() - kHeight) / 2;

   return Rectangle<int>(x, y, kWidth, kHeight);
}


void ATagButton::SetShowClose(bool doShow)
{
   fShowClose = doShow;
   this->repaint();
}


void ATagButton::SetTag(const ATag& tag) noexcept
{
   this->setButtonText(tag.Name());
   fTag = tag;
}


String ATagButton::TagName() const
{
   return fTag.Name();
}


void ATagButton::mouseDown(const MouseEvent& e)
{
   if (this->CloseBounds().contains(e.getMouseDownPosition()))
   {
      if (nullptr != this->OnCloseClick)
      {
         this->OnCloseClick();
      }
   }
   else
   {
      Button::mouseDown(e);
   }
}


void ATagButton::paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted,
                             bool shouldDrawButtonAsDown)
{
   auto& lf = getLookAndFeel();

   lf.drawButtonBackground(g, *this,
                           findColour (getToggleState() ? buttonOnColourId : buttonColourId),
                           shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);


   if (fShowClose)
   {
      auto closeImage = ImageCache::getFromMemory(BinaryData::icondeletetag_png,
                                                  BinaryData::icondeletetag_pngSize);

      auto t = RectanglePlacement(RectanglePlacement::stretchToFit)
       .getTransformToFit(closeImage.getBounds().toFloat(),
                          this->CloseBounds().toFloat());

      g.setOpacity(1.0f);
      g.drawImageTransformed(closeImage, t, false);
   }

   lf.drawButtonText(g, *this, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}
