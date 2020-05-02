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

#include <iterator>
#include <numeric>
#include <string>
#include <string_view>
#include <cstring>
#include <type_traits>
#include <vector>
#include <cassert>

namespace StringUtils
{
   // !!! Use JUCE's?
   /** \return byteCount formatted with units: B, kB, MB, GB, TB, PB, EB */
   std::string HumanBytes(int64_t byteCount);

   /**
    * Return pointers into data marking where each line begins and its length
    * not including the newline itself. Blank lines are returned.
    */
   std::vector<std::string_view> SplitLines(const char* data, size_t length);

   /**
    * Produce a string with the given values separated by the delim.
    */
   template <typename T>
   std::string Join(const std::vector<T>& values, const std::string& delim)
   {
      std::string result;
      if (!values.empty())
      {
         auto first = values.begin();
         result = std::accumulate(++first, values.end(), std::to_string(values.front()),
          [delim](std::string total, const auto& value)
          {
             total += delim;
             return total += std::to_string(value);
          });
      }
      return result;
   }

   /** std::string value specialization of Join, doesn't call to_string on values */
   template <> std::string Join(const std::vector<std::string>& values, const std::string& delim);


   // MARK: - String Builder and support classes

   struct AStrReserver final
   {
      static //constexpr
      void Reserve(std::string& str, size_t amt)
      {
         str.reserve(amt);
      }
   };

   /** Do nothing when asked to Reserve. */
   template<class T>
   struct ANullReserver final
   {
      static constexpr
      void Reserve(T&, size_t) noexcept
      {}
   };

   /**
    * Accumulate a variable list of values recursively through the += operator
    * on the accumulator. This happens to be the most efficient operator to build
    * a string.
    *
    * Reserver is a policy for reserving memory or otherwise praparing
    * the accumulator. It must respond to `void Reserve(Accum&, size_t)`
    *
    * Can be used statically, or through an instance using it as a functor.
    */
   template <class Accum, class Reserver = AStrReserver>
   class Builder final
   {
   public:
      template<class ... Args> constexpr
      void operator()(Accum& accum, Args... rest) const
      {
         Build(accum, rest...);
      }

      template<class ... Args> static constexpr
      void Build(Accum& accum, Args... rest)
      {
         // Really asking, "is Accum a string or buffered type?"
         if constexpr(!std::is_integral<Accum>() && !std::is_floating_point<Accum>())
         {
            Reserve(accum, rest...);
         }
         BuildAux(accum, rest...);
      }

   private:

      // Recursive case
      template <typename T, class ... Args>
      static constexpr
      void BuildAux(Accum& accum, const T& first, Args... rest)
      {
         accum += first;
         BuildAux(accum, rest...);
      }

      // Base case
      static constexpr
      void BuildAux(const Accum&) noexcept
      {}

      // MARK: Specializations

      /** Add the length of the next string to the total string length. */
      template <class ... Args>
      static constexpr
      void BuildAux(size_t& accum, const std::string& str, Args... rest)
      {
         accum += str.size();
         BuildAux(accum, rest...);
      }

      /** Add the length of the c-string to the total string length. */
      template <class ... Args>
      static constexpr
      void BuildAux(size_t& accum, const char* str, Args... rest)
      {
         accum += strlen(str);
         BuildAux(accum, rest...);
      }

      // MARK: Memory handling

      template<class ... Args>
      static constexpr
      void Reserve(Accum& accum, Args... rest)
      {
         // accum.reserve(Sizer::Size(rest...));
         size_t size = 0L;
         Builder<size_t, ANullReserver<size_t>>::Build(size, rest...);
         Reserver::Reserve(accum, size);
      }
   };
};
