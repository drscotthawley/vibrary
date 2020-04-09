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

#include "Spectrogram.h"


/**
 * Convert a spectrogram to an RGB image.
 *
 * Subclasses manage how the spectrogram's values are converted to RGB.
 */
class Colorizer
{
public:
   explicit Colorizer(const ASpectrogram& spectra)
   : fSpectra(spectra)
   {
      const auto range = spectra.MinMax();
      fMinVal = range.first;
      fMaxVal = range.second;
   }

   virtual ~Colorizer() 
   {}

   operator juce::Image() const { return this->AsImage(); }

   juce::Image AsImage() const
   {
      if (3 > fSpectra.ChannelCount())
      {
         return this->ColorizeSpectra();
      }
      else
      {
         return this->NormalizeSpectra();
      }
   }

   /** Output Colour should be normalized to Range(). */
   virtual Colour Colorize(float) const = 0;

   constexpr float Range() const noexcept 
   { 
      return fMaxVal - fMinVal;
   }

protected:

   constexpr float ScaleToRange(float c) const noexcept
   {
      return (c - fMinVal) * 255.0f / this->Range();
   }

   float fMinVal = 0.0f;
   float fMaxVal = 0.0f;

private:

   // Take the first channel and convert to color with Colorize(float). */
   juce::Image ColorizeSpectra() const;

   // Handle 3 channels by just normalizing.
   juce::Image NormalizeSpectra() const;
   
   const ASpectrogram& fSpectra;
};


/**
 * Assign equal amounts to RGB channels
 */
class Grayscale final : public Colorizer
{
public:
   explicit Grayscale(const ASpectrogram& spectra) : Colorizer(spectra)
   {}

   Colour Colorize(float c) const override
   {
      const float scaled = this->ScaleToRange(c);
      return Colour::fromRGBA(scaled, scaled, scaled, 1.0f);
   }
};


class Contrasty final : public Colorizer
{
public:
   explicit Contrasty(const ASpectrogram& spectra) : Colorizer(spectra)
   {}

   Colour Colorize(float c) const override
   {
#if 1 // Blue spectral on light bg

      const float normal = (c - fMinVal) / this->Range();
      return Colour::fromHSV(0.5f + normal * 0.33f, // Blue shades
                             normal * 0.9f,
                             0.9f,
                             1.0f);

#else // Magenta on black

      const float limitedHue = 0.66f + normal * 0.66f;
      return Colour::fromHSV(limitedHue <= 1.0f ? limitedHue : 1.0f - limitedHue,
                             0.9f,
                             // Boost the value a bit for brighter thumbs
                             0.1f + (c - fMinVal) * 0.9f / (this->Range() - 0.1f),
                             1.0f);
#endif
   }
};
