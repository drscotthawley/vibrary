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

#include <cassert>
#include <cmath>

#include "Float16.h"

namespace
{
   constexpr static const float sSignLut[] { 1.0f, -1.0f };
}


constexpr uint16_t Float16::kBias;
constexpr uint16_t Float16::kNormal;


Float16::Float16(uint16_t bytes)
{
   significand = (bytes >>  0U) & 0x03ff;
   exponent    = (bytes >> 10U) & 0x001f;
   sign        = (bytes >> 15U) & 0x0001;
}


Float16::Float16(float f)
{
   *this = f;
}

Float16 Float16::operator=(float single)
{
   union Float32 { float f32; uint32_t u32; } f { single };
   
   sign = (f.u32 & 0x80000000) >> 31;
   
   if (0x00000000 == (f.u32 & 0x7fffffff))
   {
      // +/-0
      exponent = 0;
      significand = 0;
   }
   else
   {
      const uint32_t fexpo = (f.u32 & 0x7f800000);
      // Mask signifier and add hidden bit
      uint32_t fsgnf = (f.u32 & 0x007fffff) | (0x00000001 << 23);
      if (0x7f800000 == fexpo)
      {
         // +/-inf (significand == 0)
         // +/-NaN (significand != 0)
         exponent = 0x1F;
         fsgnf >>= 13;
      }
      else
      {
         const int32_t hexpo = (fexpo >> 23) - (127U - kBias);
  
         if (0 >= hexpo) // denormal
         {
            // For each negative exponent, shift our significand an extra bit.
            exponent = 0;
            fsgnf >>= 13 + (1 - hexpo);
         }
         else
         {
            exponent = static_cast<uint32_t>(hexpo);
            fsgnf >>= 13;
         }
      }
      significand = fsgnf;
   }
   return *this;
}


Float16::operator float() const
{
   // Special values
   if (0b00011111 == exponent)
   {
      // return silent +/-NaN or +/-inf
      return sSignLut[sign] * (0x000 != significand ? ::nanf(nullptr) : INFINITY);
   }

   // normal: -1**S * 2**(e-15) * 1.significand
   int sexponent = (int)exponent - kBias;
   float fraction = 1.0f;
   
    // if subnormal: -1**S * 2**-14 * 0.significand
   if (0b00000 == exponent)
   {
      sexponent = -14;
      fraction = 0.0f;
   }
   fraction += (float)significand / kNormal;

   return sSignLut[sign] * ::powf(2.0f, sexponent) * fraction;
};

