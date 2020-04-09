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
#include <iostream>
#include <memory>

#include "sqlite3.h"
#include "Database.h"
#include "DatabaseCommands.h"


ADatabase::ADatabase(const std::string& filename)
: fDb(nullptr)
{
   this->Open(filename);
}


ADatabase::~ADatabase()
{
   this->Close();
}


void ADatabase::Close()
{
   if (nullptr != fDb)
   {
      // TODO: Close blob handles
      // TODO: Finalize prepared statements, (if needed, but we use one step execution so far)
      // TODO: Finish backup statements
      int result = sqlite3_close(reinterpret_cast<sqlite3*>(fDb));
      if (SQLITE_BUSY == result)
      {
         std::cerr << "WARNING: Some pending database transactions may be rolled "
                      "back due to closing the database with pending statements.\n";
      }

      fDb = nullptr;
   }
}


bool ADatabase::ConstraintUnique(int result) noexcept
{
   return SQLITE_CONSTRAINT_UNIQUE == result;
}


int64_t ADatabase::LastInsertedRowId() const
{
   assert(nullptr != fDb);
   return sqlite3_last_insert_rowid(reinterpret_cast<sqlite3*>(fDb));
}


const std::string& ADatabase::LastErrorMessage() const noexcept
{
   return fLastErrorMessage;
}


int ADatabase::Execute(const ADatabaseCommand& command)
{
   assert(nullptr != fDb);
   sqlite3* db = reinterpret_cast<sqlite3*>(fDb);
   
   fLastErrorMessage.clear();
   char* errorMessage = nullptr;
   int result = sqlite3_exec(db, command.AsSql().c_str(),
                             (sqlite3_callback)command.ResultCallback(), (void*)&command,
                             &errorMessage);
   auto msgGuard = std::unique_ptr<char, decltype(&sqlite3_free)>(errorMessage,
                                                                  sqlite3_free);
   
   if (SQLITE_OK != result)
   {
      fLastErrorMessage = errorMessage;
      std::cerr << "ERROR: " << errorMessage << "\n";
      std::cerr << command.AsSql() << "\n";
   }
   
   return result;
}


int ADatabase::Open(const std::string& filename)
{
   auto dbPtr = reinterpret_cast<sqlite3**>(&fDb);
   auto result = sqlite3_open(filename.c_str(), dbPtr);
   if (SQLITE_OK != result)
   {
      std::cerr << "Cannot open database: " << sqlite3_errmsg(*dbPtr);
   }
   else
   {
      sqlite3_extended_result_codes(*dbPtr, (int)true);
   }
   return result;
}



// MARK: -

int ATransaction::sNextTransactionId = kNoTransactionActive;
int ATransaction::sTransactionActive = kNoTransactionActive;

ATransaction::ATransaction(ADatabase* db)
: fDb(db)
, fRolledBack(false)
, fTransactionId(++sNextTransactionId)
{
   assert(nullptr != fDb);
   
   if (ATransaction::NothingActive())
   {
      this->Activate();
      fDb->Execute(ABeginTransaction());
   }
}

ATransaction::~ATransaction()
{
   if (this->IsActive() && !fRolledBack)
   {
      fDb->Execute(AEndTransaction());
      this->Deactivate();
   }
}

void ATransaction::Activate()
{
   sTransactionActive = fTransactionId;
}

void ATransaction::Deactivate()
{
   sTransactionActive = kNoTransactionActive;
}

bool ATransaction::IsActive() const
{
   return sTransactionActive == fTransactionId;
}

bool ATransaction::NothingActive()
{
   return kNoTransactionActive == sTransactionActive;
}

void ATransaction::Rollback()
{
   if (this->IsActive())
   {
      this->Deactivate();
      fRolledBack = (0 == fDb->Execute(ARollbackTransaction()));
   }
}
