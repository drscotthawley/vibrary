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

#include "Colorizers.h"
#include <cassert>

juce::Image Colorizer::ColorizeSpectra() const
{
   const auto [height, width] = fSpectra.Shape();
   assert(width > 0 && height > 0 && "Image sizes must be positive");

   assert(fSpectra.ChannelCount() <= 3 && "ColorizeSpectra only handles 3 or fewer channels.");

   auto image = juce::Image{Image::PixelFormat::ARGB, (int)width, (int)height, true};
   auto bitmap = juce::Image::BitmapData{image, Image::BitmapData::writeOnly};

   const float* source = fSpectra.Channel(0).data();

   // flip as we go
   for (int y = height - 1; y >= 0; --y)
   {
      auto rowIt = reinterpret_cast<PixelARGB*>(bitmap.getLinePointer(y));
      auto rowEnd = rowIt + width;
      while (rowEnd != rowIt)
      {
         // Scale from the data's range to uint8's.
         const auto color = this->Colorize(*source++);
         (rowIt++)->setARGB(color.getAlpha(),
                            color.getRed(),
                            color.getGreen(),
                            color.getBlue());
      }
   }

   return image;
}

juce::Image Colorizer::NormalizeSpectra() const
{
   int width;
   int height;
   std::tie(height, width) = fSpectra.Shape();
   assert(width > 0 && height > 0 && "Image sizes must be positive");

   auto image { juce::Image(Image::PixelFormat::ARGB, width, height, true) };
   auto bitmap { juce::Image::BitmapData(image, Image::BitmapData::writeOnly) };

   assert(fSpectra.ChannelCount() >= 3 && "NormalizeSpectra only handles multichannel audio.");

   auto sources { std::vector<const float*>(3, fSpectra.Channel(0).data()) };
   for (int i = 0; i < 3; ++i)
   {
      sources[i] = fSpectra.Channel(i).data();
   }

   // flip as we go
   for (int y = height - 1; y >= 0; --y)
   {
      auto rowIt = reinterpret_cast<PixelARGB*>(bitmap.getLinePointer(y));
      auto rowEnd = rowIt + width;
      while (rowEnd != rowIt)
      {
         // Scale from the data's range to uint8's.
         (rowIt++)->setARGB(0xFF,
                            this->ScaleToRange(*sources[0]++),
                            this->ScaleToRange(*sources[1]++),
                            this->ScaleToRange(*sources[2]++));
      }
   }

   return image;
}
