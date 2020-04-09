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
#include <cstdint>

/**
 * IEEE754 half precision floating point number.
 * (For converting to/from NumPy's float16.)
 * WARN: Conversions might be relatively slow. There are LUT possibilities if needed.
 */
struct Float16
{
   // Bitfields are weird. To get the sign at bit 31 it needs to be declared last
   // (which makes some sense when you think about it).
   uint16_t significand:10;
   uint16_t exponent:5;
   uint16_t sign:1;
   
   operator float() const;
   
   Float16(float singlePrecision);
   Float16 operator=(float singlePrecision);
   explicit Float16(uint16_t inBytes);
   
   static constexpr uint16_t kBias = 15U;
   static constexpr uint16_t kNormal = 0x00000001 << 10U;
};
