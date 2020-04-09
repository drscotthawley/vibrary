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

#include "../JuceLibraryCode/JuceHeader.h"

#include "Float16.h"

#if UNIT_TESTS

namespace
{
   class AFloat16ToFloatTests : public juce::UnitTest
   {
   public:
      AFloat16ToFloatTests() : juce::UnitTest("Float16 from float tests") {}
      void runTest() override
      {
         this->beginTest("smallest positive subnormal float16");
         Float16 a((uint16_t)0x0001);
         float converted = a;
         this->expectWithinAbsoluteError(converted, 0.000000059605f, 1e-11f);
         
         this->beginTest("largest subnormal float16");
         a = Float16((uint16_t)0x03FF);
         converted = a;
         this->expectWithinAbsoluteError(converted, 0.000060976f, 1e-9f);
         
         this->beginTest("smallest positive normal float16");
         a = Float16((uint16_t)0x0400);
         converted = a;
         this->expectWithinAbsoluteError(converted, 0.000061035f, 1e-9f);
         
         this->beginTest("largest normal float16");
         a = Float16((uint16_t)0x7bff);
         converted = a;
         this->expectWithinAbsoluteError(converted, 65504.0f, 1e-11f);
         
         this->beginTest("largest number less than one float16");
         a = Float16((uint16_t)0x3bff);
         converted = a;
         this->expectWithinAbsoluteError(converted, 0.99951f, 1e-5f);
         
         this->beginTest("one float16");
         a = Float16((uint16_t)0x3c00);
         converted = a;
         this->expectWithinAbsoluteError(converted, 1.0f, 1e-11f);
         
         this->beginTest("smallest number larger than one float16");
         a = Float16((uint16_t)0x3c01);
         converted = a;
         this->expectWithinAbsoluteError(converted, 1.001f, 1e-4f);
         
         this->beginTest("-2 float16");
         a = Float16((uint16_t)0xc000);
         converted = a;
         this->expectWithinAbsoluteError(converted, -2.0f, 1e-11f);
         
         this->beginTest("zero float16");
         a = Float16((uint16_t)0x0000);
         converted = a;
         this->expectWithinAbsoluteError(converted, 0.0f, 1e-11f);
         
         this->beginTest("minus zero float16");
         a = Float16((uint16_t)0x8000);
         converted = a;
         this->expectWithinAbsoluteError(converted, -0.0f, 1e011f);
         
         this->beginTest("inf float16");
         a = Float16((uint16_t)0x7c00);
         converted = a;
         this->expect(std::isinf(converted) && std::abs(converted) == converted);
         
         this->beginTest("minus inf float16");
         a = Float16((uint16_t)0xfc00);
         converted = a;
         this->expect(std::isinf(converted) && std::abs(converted) != converted);
      }
   };
   
   
   class AFloatToFloat16Tests : public juce::UnitTest
   {
   public:
      AFloatToFloat16Tests() : juce::UnitTest("float to Float16 tests") {}
      void runTest() override
      {
         this->beginTest("Special float values");
         union F16 { Float16 f16; uint16_t u16; } test { 0.0f };
         this->expectEquals(test.u16, (uint16_t)0x0000);
         
         test.f16 = -0.0f;
         this->expectEquals(test.u16, (uint16_t)0x8000);

         test.f16 = INFINITY;
         this->expectEquals(test.u16, (uint16_t)0x7c00);
         
         test.f16 = -INFINITY;
         this->expectEquals(test.u16, (uint16_t)0xfc00);
         
         test.f16 = ::nanf(nullptr); // NaN
         this->expectEquals((uint16_t)(test.u16 & 0x7c00), (uint16_t)0x7c00);
         this->expectNotEquals((uint16_t)(test.u16 & 0x03ff), (uint16_t)0x0000);
         
         test.f16 = -test.f16; // -NaN
         this->expectEquals((uint16_t)(test.u16 & 0xfc00), (uint16_t)0xfc00);
         this->expectNotEquals((uint16_t)(test.u16 & 0x03ff), (uint16_t)0x0000);
         
         this->beginTest("largest normal float16");
         test.f16 = 65504.0f;
         this->expectEquals(test.u16, (uint16_t)0x7bff);
         
         this->beginTest("smallest positive normal float16");
         test.f16 = 0.0000610351563f; //0.000061035f;
         this->expectEquals(test.u16, (uint16_t)0x0400);
         
         this->beginTest("one float16");
         test.f16 = 1.0f;
         this->expectEquals(test.u16, (uint16_t)0x3c00);
         
         this->beginTest("largest number less than one float16");
         test.f16 = 0.999511718; //0.99951f;
         this->expectEquals(test.u16, (uint16_t)0x3bff);
         
         this->beginTest("smallest number larger than one float16");
         test.f16 = 1.001f;
         this->expectEquals(test.u16, (uint16_t)0x3c01);
         
         this->beginTest("-2 float16");
         test.f16 = -2.0f;
         this->expectEquals(test.u16, (uint16_t)0xc000);
         
         this->beginTest("largest subnormal float16");
         test.f16 = 0.0000609755516f;
         this->expectEquals(test.u16, (uint16_t)0x03ff);
         
         this->beginTest("smallest positive subnormal float16");
         test.f16 = 0.0000000596046448f;//0.000000059605f;
         this->expectEquals(test.u16, (uint16_t)0x0001);
      }
   };
   
   static AFloat16ToFloatTests sFloatToFloat16Tests;
   static AFloatToFloat16Tests sFloat16ToFloatTests;
}

#endif
