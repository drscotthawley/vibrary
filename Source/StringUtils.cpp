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
#include <limits>
#include <numeric>
#include <sstream>
#include <vector>

#include "StringUtils.h"

namespace StringUtils
{
   std::string HumanBytes(int64_t byteCount)
   {
      constexpr uint64_t kUnit = 1000LL;
      const char* const sOrder = "BkMGTPE";

      int mag = 0LL;
      
//#define FLOAT_IMPL_HumanBytes
#if FLOAT_IMPLE_HumanBytes // can have trouble due to precision, but is faster.

      if (0 != byteCount)
      {
         mag = std::log10l(std::fabsl((long double)byteCount)) / std::log10l(kUnit);
         
         assert(mag < strlen(sOrder) && mag >= 0);
      }
      
#else // Integer version
      
      int64_t temp = std::llabs(byteCount);
      while (temp >= kUnit && mag < ::strlen(sOrder))
      {
         temp /= kUnit;
         mag += 1LL;
      }

#endif

      byteCount = byteCount / std::powl(kUnit, mag);

      std::stringstream result;
      result << byteCount << " " << sOrder[mag];
      if (mag > 0) // add B if we didn't already
      {
         result << sOrder[0];
      }
     
      return result.str();
   }


   template <>
   std::string Join(const std::vector<std::string>& values, const std::string& delim)
   {
      std::string result;
      if (!values.empty())
      {
         auto first = values.begin();
         result = std::accumulate(++first, values.end(), values.front(),
          [delim](std::string total, const auto& value)
          {
             total += delim;
             return total += value;
          });
      }
      return result;
   }


   std::vector<std::string_view> SplitLines(const char* data, size_t length)
   {
      std::vector<std::string_view> lines;

      if (nullptr == data)
      {
         return lines;
      }

      size_t gpos = 0;
      size_t lastStart = 0;
      while (gpos < length)
      {
         // If string terminated, grab the remainder unless a line was just finished.
         if (*(data + gpos) == '\0' && lastStart != gpos)
         {
            lines.emplace_back(data + lastStart, gpos - lastStart);
         }

         // Allows blank lines which will have length 0.
         if (*(data + gpos) == '\n')
         {
            lines.emplace_back(data + lastStart, gpos - lastStart);
            lastStart = gpos + 1;
         }
         gpos += 1;
      }
      return lines;
   }
};



#if UNIT_TESTS

#include "../JuceLibraryCode/JuceHeader.h"


namespace
{
   namespace su = StringUtils;
   
   class AHumanBytesTest : public juce::UnitTest
   {
   public:
      AHumanBytesTest() : juce::UnitTest("Human readable bytes")
      {}

      void runTest() override
      {
         this->beginTest("test boundaries");
         
         this->expectEquals(su::HumanBytes(0LL),  std::string("0 B"));
         this->expectEquals(su::HumanBytes(-1LL), std::string("-1 B"));
         this->expectEquals(su::HumanBytes(1LL),  std::string("1 B"));
         
         this->expectEquals(su::HumanBytes(999LL),  std::string("999 B"));
         this->expectEquals(su::HumanBytes(1000LL), std::string("1 kB"));
         this->expectEquals(su::HumanBytes(1001LL), std::string("1 kB"));
         this->expectEquals(su::HumanBytes(10000LL), std::string("10 kB"));
         this->expectEquals(su::HumanBytes(1000000LL - 1LL), std::string("999 kB")); // 1e6
         
         this->expectEquals(su::HumanBytes(1000000LL),          std::string("1 MB"));
         this->expectEquals(su::HumanBytes(1000000000LL - 1LL), std::string("999 MB")); // 1e9
         this->expectEquals(su::HumanBytes(1000000000LL),          std::string("1 GB"));
         this->expectEquals(su::HumanBytes(1000000000000LL - 1LL), std::string("999 GB")); // 1e12
         this->expectEquals(su::HumanBytes(1000000000000LL),          std::string("1 TB"));
         this->expectEquals(su::HumanBytes(1000000000000000LL - 1LL), std::string("999 TB")); //1e15
         this->expectEquals(su::HumanBytes(1000000000000000LL),          std::string("1 PB"));
#if FLOAT_IMPL_HumanBytes // this one needs more "room" due to floating point precision
         this->expectEquals(su::HumanBytes(1000000000000000000LL - 1LL), std::string("999 PB"));
#else
         this->expectEquals(su::HumanBytes(1000000000000000000LL - 1LL), std::string("999 PB")); // // 1e18
#endif
         this->expectEquals(su::HumanBytes(1000000000000000000LL), std::string("1 EB"));
         
         this->beginTest("test limits");
         
         using i64limits = std::numeric_limits<int64_t>;
         this->expectEquals(su::HumanBytes(i64limits::max()), std::string("9 EB"));
         this->expectEquals(su::HumanBytes(i64limits::min()), std::string("-9 EB"));
      }
   };


   class AJoinTest : public juce::UnitTest
   {
   public:
      AJoinTest() : juce::UnitTest("Join string test")
      {}

      void runTest() override
      {
         using str = std::string;

         this->expectEquals(su::Join(std::vector<str>{}, "anything"), str());
         this->expectEquals(su::Join(std::vector<double>{}, "anything"), str());
         this->expectEquals(su::Join(std::vector<str>{ "start" }, "anything"), str("start"));
         this->expectEquals(su::Join(std::vector<str>{ "A", "B", "C" }, ", "),
                            str("A, B, C"));
         this->expectEquals(su::Join(std::vector{ 1LL, 2LL, 3LL, 4LL }, ","), str("1,2,3,4"));
      }
   };

   class ASplitLinesTest : public juce::UnitTest
   {
   public:
      ASplitLinesTest() : juce::UnitTest("Split Lines test")
      {}

      void runTest() override
      {
         std::string buffer;
         buffer.reserve(4096);

         this->beginTest("simple lines");

         buffer = "Mon\nTue\nWed\nThu\nFri\nSat\nSun\n";
         auto lines = su::SplitLines(buffer.data(), buffer.size());
         this->expectEquals(lines.size(), 7UL);
         this->expect("Mon" == lines[0]);
         this->expect("Sun" == lines[6]);
         for (const auto& line : lines)
         {
            this->expectEquals(line.size(), 3UL);
         }

         this->beginTest("empty lines");

         buffer = "Mon\n\nTue\n\nWed\n\nThu\n\nFri\n\nSat\n\nSun\n\n";
         lines = su::SplitLines(buffer.data(), buffer.size());
         this->expectEquals(lines.size(), 14UL);
         this->expect("Mon" == lines[0]);
         this->expectEquals(lines[1].size(), 0UL);
         this->expectEquals(lines[7].size(), 0UL);
         this->expect("Fri" == lines[8]);
         for (size_t i = 0; i < lines.size(); i += 2)
         {
            this->expectEquals(lines[i].size(), 3UL);
            this->expectEquals(lines[i + 1].size(), 0UL);
         }

         this->beginTest("c-string-terminated last line");

         buffer = "Mon\nTue\nThu";
         lines = su::SplitLines(buffer.c_str(), buffer.size() + 1);
         this->expectEquals(lines.size(), 3UL);
         this->expect("Thu" == lines[2]);
         for (const auto& line : lines)
         {
            this->expectEquals(line.size(), 3UL);
         }

         this->beginTest("unterminated last line");

         auto rawBuff = std::array<char, 7> { 'M', 'o', 'n', '\n', 'T', 'u', 'e' };
         lines = su::SplitLines(rawBuff.data(), rawBuff.size());
         this->expectEquals(lines.size(), 1UL);
         this->expect("Mon" == lines[0]);

         this->beginTest("no lines, no termination");

         rawBuff = { 'n', 'o', ' ', 'l', 'i', 'n', 'e' };
         lines = su::SplitLines(rawBuff.data(), rawBuff.size());
         this->expectEquals(lines.size(), 0UL);

         this->beginTest("empty buffer");

         buffer = "";
         lines = su::SplitLines(buffer.data(), buffer.size());
         this->expectEquals(lines.size(), 0UL);

         this->beginTest("nullptr buffer");

         lines = su::SplitLines(nullptr, buffer.capacity());
         this->expectEquals(lines.size(), 0UL);
      }
   };


   class ABuildTest : public juce::UnitTest
   {
   public:
      ABuildTest() : juce::UnitTest("String Build test")
      {}

      void runTest() override
      {
         this->beginTest("zero arguments");
         
         namespace su = StringUtils;
         std::string result;
         const size_t minCap = result.capacity();
         
         su::Builder<std::string> builder;
         
         builder(result);
         this->expect(result.empty());
         this->expectEquals(minCap, result.capacity());
         
         this->beginTest("bases");
         const char* const cTest = "test";
         const auto test = std::string(cTest);
         assert(minCap > test.size() * 3L);
         
         builder(result, test);
         this->expectEquals(result, test);
         this->expectEquals(result.size(), test.size());
         this->expectEquals(result.capacity(), minCap); // no growth
         
         result.clear();
         builder(result, cTest);
         this->expectEquals(result, test);
         this->expectEquals(result.size(), test.size());
         this->expectEquals(result.capacity(), minCap); // no growth
         
         this->beginTest("recursion");
         
         result = std::string();
         builder(result, test, test, test);
         this->expectEquals(result, test + test + test);
         this->expectEquals(result.size(), test.size() * 3L);
         this->expectEquals(result.capacity(), minCap);
         
         result.clear();
         builder(result, test, " ", test);
         this->expectEquals(test + " " + test, result);
         this->expectEquals(result.size(), test.size() * 2L + 1L);
         this->expectEquals(result.capacity(), minCap);
         
         this->beginTest("resize capacity");
         
         builder(result, test, test, test, test, test, test);
         this->expectEquals(test.size() * 8L + 1, result.size());
         this->expectLessThan(minCap, result.capacity());
      }
   };
   
   static AHumanBytesTest sHumanBytesTest;
   static ASplitLinesTest sSplitLinesTest;
   static ABuildTest sBuildTest;
   
}

#endif
