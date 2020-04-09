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

#include "AppLookAndFeel.h"


Typeface::Ptr AAppLookAndFeel::sMontserrat;


const Colour AAppLookAndFeel::kLightGray     = PixelARGB(0xFF, 0xEC, 0xEC, 0xEC);
const Colour AAppLookAndFeel::kMediumGray    = PixelARGB(0xFF, 0x66, 0x66, 0x66);
const Colour AAppLookAndFeel::kDarkGray      = PixelARGB(0xFF, 0x22, 0x22, 0x22);
const Colour AAppLookAndFeel::kGreenProgress = PixelARGB(0xFF, 0x69, 0xC2, 0x7A);
const Colour AAppLookAndFeel::kPurpleNav     = PixelARGB(0xFF, 0x50, 0x74, 0xDD);


AAppLookAndFeel::AAppLookAndFeel()
{
   auto colourScheme = this->getCurrentColourScheme();
   colourScheme.setUIColour(ColourScheme::defaultText, kMediumGray);
   colourScheme.setUIColour(ColourScheme::menuText, kMediumGray);
   colourScheme.setUIColour(ColourScheme::menuBackground, kLightGray);
   colourScheme.setUIColour(ColourScheme::defaultFill, kDarkGray);
   colourScheme.setUIColour(ColourScheme::widgetBackground, kLightGray);

   this->setColourScheme(colourScheme);
   
   this->setColour(ListBox::backgroundColourId, Colours::white);
   this->setColour(ListBox::textColourId, kMediumGray);

   this->setDefaultSansSerifTypeface(this->GetMontserratFace());
}


void AAppLookAndFeel::CleanUp()
{
   sMontserrat = nullptr;
}


Typeface::Ptr AAppLookAndFeel::GetMontserratFace()
{
   if (nullptr == sMontserrat)
   {
      sMontserrat = Typeface::createSystemTypefaceFor(BinaryData::MontserratMedium_otf,
                                                      BinaryData::MontserratMedium_otfSize);
   }
   return sMontserrat;
}

