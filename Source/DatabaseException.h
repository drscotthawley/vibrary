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

#include <stdexcept>
#include <string>

/**
 * Exception thrown from database operations
 */
class ADatabaseException final : public std::runtime_error
{
public:
   ADatabaseException(const char* what, int result)
   : std::runtime_error(what)
   , fResult(result)
   {}

   ADatabaseException(const std::string& what, int result)
   : std::runtime_error(what)
   , fResult(result)
   {}

   /**
    * The result returned when a command was executed by the database.
    * $$$ Interpret as a sqlite error, which leaks the db imp to the public
    * so fix with our own version of the codes later.
    */
   constexpr int Result() const noexcept { return fResult; }

private:
   const int fResult;
};
