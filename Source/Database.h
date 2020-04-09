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

class ADatabaseCommand;

class ADatabase
{
public:
   ADatabase(const std::string& filename); //!< Calls Open()
   ~ADatabase(); //!< Calls Close()

   /** \returns 0 on success */
   int Execute(const ADatabaseCommand& command);
   
   /**
    * The primary key of the last inserted record. Useful for getting result
    * of insterting single items.
    */
   int64_t LastInsertedRowId() const;
   
   const std::string& LastErrorMessage() const noexcept;
   
   static bool ConstraintUnique(int result) noexcept;

private:
   
   int Open(const std::string& filename);
   void Close();

   void* fDb;
   std::string fLastErrorMessage;
};


/**
 * Group sets of commands executed during its lifetime into a transaction.
 *
 * NOT THREAD SAFE YET
 *
 * It takes care to prevent multiple transactions from starting but allows
 * nesting commands that have their own transaction start-up code.
 */
class ATransaction
{
public:
   /** \param db Ownership of db is NOT transferred */
   ATransaction(ADatabase* db);
   ~ATransaction();
   void Rollback();
private:
   ADatabase* fDb;
   bool fRolledBack;

   // Helpers to keep only one active transaction going in the database.
   // Allows nesting commands which each have their own
   void Activate();
   bool IsActive() const;
   void Deactivate();
   static bool NothingActive();
   
   static constexpr int kNoTransactionActive = 0;
   int fTransactionId;
   static int sNextTransactionId;
   static int sTransactionActive;
};
