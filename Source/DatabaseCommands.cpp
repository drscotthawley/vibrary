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

#include <numeric>
#include <stdexcept>

#include "DatabaseCommands.h"
#include "StringUtils.h"

using SBuilder = StringUtils::Builder<std::string>;
using namespace Trigger;

namespace
{
   int AppendResultMap(void* command, int columnCount, char** values, char** names);
}


// MARK: ADatabaseCommand

ADatabaseCommand::~ADatabaseCommand()
{}

void ADatabaseCommand::AddRowResult(const RowResult& result)
{
   fResults.push_back(result);
}

void ADatabaseCommand::AddRowResult(const RowResult&& result)
{
   fResults.push_back(std::move(result));
}

ADatabaseCommand::operator std::string() const
{
   auto statement = this->AsSql();
   return statement.replace(statement.length() - 1, 1, " ");
}

Database::Callback ADatabaseCommand::ResultCallback() const
{
   return AppendResultMap;
}

const RowResult& ADatabaseCommand::Result(size_t rowIndex) const
{
   return fResults.at(rowIndex);
}

const CommandResults& ADatabaseCommand::Results() const
{
   return fResults;
}


// MARK: -

AInsertCommand::AInsertCommand(const std::string& table, const Strings& values)
: fTableName(table)
, fValues(values)
{
   if (fValues.empty())
   {
      throw std::runtime_error("Cannot INSERT an empty value list.");
   }
}


AInsertCommand::AInsertCommand(const std::string& table, const Strings& values,
                               const Strings& columns)
: AInsertCommand(table, values)
{
   if (columns.size() != fValues.size())
   {
      throw std::runtime_error("An INSERT with column names must have the same number of values.");
   }
   fColumns = columns;
}

std::string AInsertCommand::AsSql() const
{
   using namespace std;
   
   string cmd;
   cmd.reserve(this->EstimateCommandSize());
   
   cmd += "INSERT INTO " + fTableName + " ";
   
   if (!fColumns.empty())
   {
      auto columnIt = begin(fColumns);
      const auto columnEnd = end(fColumns);
      
      cmd += "( ";
      cmd += *columnIt++;
      while (columnEnd != columnIt)
      {
         cmd += ", ";
         cmd += *columnIt++;
      }
      cmd += " ) ";
   }
   
   auto valueIt = begin(fValues);
   const auto valueEnd = end(fValues);
   cmd += "VALUES ( ";
   cmd += *valueIt++;
   while (valueEnd != valueIt)
   {
      cmd += ", ";
      cmd += *valueIt++;
   }
   cmd += " );";
   
   return cmd;
}

size_t AInsertCommand::EstimateCommandSize() const
{
   using namespace std;
   
   constexpr size_t kBasicCommandSize = 22L;
   constexpr size_t kSepartorConsts = 2L;
   
   auto addNameSize = [](size_t current, const auto& name)
   {
      return current + kSepartorConsts + name.size();
   };
   
   size_t size = kBasicCommandSize;
   accumulate(begin(fColumns), end(fColumns), size, addNameSize);
   return size;
}


// MARK: -

ACreateTableCommand::ACreateTableCommand(const std::string& tableName)
: fTableName(tableName)
{}

void ACreateTableCommand::AddColumn(const std::string& name)
{
   fColumns.push_back(name);
}

void ACreateTableCommand::AddColumn(const std::string&& name)
{
   fColumns.push_back(std::move(name));
}

void ACreateTableCommand::AddColumn(const AColumn& column)
{
   this->AddColumn(static_cast<std::string>(column));
}

void ACreateTableCommand::AddColumn(const AColumn&& column)
{
   this->AddColumn(static_cast<std::string>(column));
}

void ACreateTableCommand::AddColumns(const ColumnNames& names)
{
   fColumns = names;
}

void ACreateTableCommand::AddColumns(const ColumnNames&& names)
{
   fColumns = std::move(names);
}

void ACreateTableCommand::AddRelation(const std::string& name, const std::string& toTable,
                 const std::string& targetColumn)
{
   //fRelationships.emplace(name, toTable, targetColumn);
   fRelationships[name] = std::make_pair(toTable, targetColumn);
}

std::string ACreateTableCommand::AsSql() const
{
   using namespace std;
   
   bool closeColumns = false;
   string cmd;
   cmd.reserve(this->EstimateCommandSize());
   
   cmd += "CREATE TABLE IF NOT EXISTS " + fTableName;
   
   if (!fColumns.empty() || !fRelationships.empty())
   {
      cmd += " (";
      closeColumns = true;
   }
   auto separator = "";
   for (auto column : fColumns)
   {
      cmd += separator + column;
      // If the caller didn't specify the type, use TEXT.
      if (string::npos == column.find_first_of(" "))
      {
         cmd += " TEXT";
      }
      separator = ", ";
   }
   
   for (const auto& entry : fRelationships)
   {
      const auto& name = entry.first;
      const auto& toTable = entry.second.first;
      const auto& targetColumn = entry.second.second;
      
      SBuilder::Build(cmd, ", "
       "FOREIGN KEY(", name, ") REFERENCES ", toTable, "(", targetColumn, ")");
   }
   
   if (closeColumns)
   {
      cmd += ")";
   }
   cmd += ";\n";

   for(const auto& entry : fRelationships)
   {
      const auto& name = entry.first;
      SBuilder::Build(cmd, "\n"
       "CREATE INDEX IF NOT EXISTS ",  fTableName, "_", name,
       " ON ", fTableName, "(", name, ");");
   }
   
   return cmd;
}

size_t ACreateTableCommand::EstimateCommandSize() const
{
   using namespace std;
   
   // Basic command values.
   size_t size = 27L; // CREATE TABLE...
   
   // All uses of table name
   size += fTableName.size() * (fRelationships.size() * 2 + 1);
   
   // Column names and separates
   accumulate(fColumns.begin(), fColumns.end(), size,
    [](size_t current, const auto& name)
    {
       // If no type " TEXT" is used, so search for space separator of name and type
       return current
        + name.size() + 2L
        + (string::npos == name.find_first_of(" ") ? 5L : 0L);
    });
   
   // Constant strings for relationships and index commands
   constexpr size_t kForeignKeyConsts = 32L;
   constexpr size_t kIndexConsts = 35L;
   size += fRelationships.size() * (kForeignKeyConsts + kIndexConsts);
   
   accumulate(fRelationships.begin(), fRelationships.end(), size,
    [](size_t current, const auto& columnTarget)
    {
       return current
        + (columnTarget.first.size() * 2L)
        + columnTarget.second.first.size()
        + columnTarget.second.second.size();
    });

   return size;
}

// MARK: -


ACreateTriggerCommand::ACreateTriggerCommand(const std::string& name,
  const std::string& onTable, Aspect aspect, Command triggerByCommand,
  IsTemp temp, ForEachRow doForEachRow,
  const std::vector<std::string>& triggeredCommands)
: fName{name}
, fTableName{onTable}
, fCommands{StringUtils::Join(triggeredCommands, "; ") += ";"}
, fTemporary{temp}
, fForEachRow{doForEachRow}
{
   fStatement.emplace_back(kAspect, AspectName(aspect));
   fStatement.emplace_back(kTrigByCommand, TriggerCommandName(triggerByCommand));
}


std::string ACreateTriggerCommand::AsSql() const
{
   std::string whenClause;
   if (fStatement.size() == 3)
   {
      whenClause.reserve(6 + fStatement[kWhen].second.size());
      whenClause += "WHEN ";
      whenClause += fStatement[kWhen].second;
   }

   std::string endCommands = std::string(";");
   if (fCommands.back() == endCommands.back())
   {
      endCommands.clear();
   }

   std::string cmd;
   SBuilder::Build(cmd,
    "CREATE ",
    fTemporary ? "TEMP " : " ",
    "TRIGGER ",
    fName, " ",
    fStatement[kAspect].second, " ",
    fStatement[kTrigByCommand].second, " ",
    "ON ", fTableName, " ",
    fForEachRow ? "FOR EACH ROW " : " ",
    whenClause, " ",
    "BEGIN ", fCommands, " END"
   );

   return cmd;
}


const char* ACreateTriggerCommand::AspectName(Aspect aspect) const
{
   switch (aspect)
   {
      case kAfter:   return "AFTER";
      case kBefore:  return "BEFORE";
      case kInstead: return "INSTEAD OF";
      default:       return "";
   }
}


const char* ACreateTriggerCommand::TriggerCommandName(Command triggerByCommand)
{
   switch (triggerByCommand)
   {
      case kDelete: return "DELETE";
      case kInsert: return "INSERT";
      case kUpdate: return "UPDATE";
      default:      return "";
   }
}


ACreateTriggerCommand ACreateTriggerCommand::When(const std::string& condition)
{
   assert(!condition.empty());
   fStatement.emplace_back(kWhen, condition);
   return *this;
}

// MARK: -

ADeleteFromTable::ADeleteFromTable(const std::string& table,
                                   const std::string& where/*=std::string()*/)
: fTableName{table}
, fCondition{where}
{}

std::string ADeleteFromTable::AsSql() const
{
   std::string cmd;
   if (fCondition.empty())
   {
      SBuilder::Build(cmd, "DELETE FROM ", fTableName, ";");
   }
   else
   {
      SBuilder::Build(cmd, "DELETE FROM ", fTableName,
                           " WHERE ", fCondition, ";");
   }
   return cmd;
}


// MARK: -

APragmaCommand::APragmaCommand(const std::string& name, const std::string& value)
{
   fCommand = std::string();
   SBuilder::Build(fCommand, "PRAGMA ", name, " = ", value, ";");
}

std::string APragmaCommand::AsSql() const
{
   return fCommand;
}


// MARK: -

constexpr size_t ASelectFromTableCommand::kNoPaging;

ASelectFromTableCommand::ASelectFromTableCommand(const std::string& tableName)
: fColumnNames({ "*" })
, fTableName(tableName)
{}

ASelectFromTableCommand::ASelectFromTableCommand(const std::string& tableName,
                                                 const ColumnNames& columnNames)
: fColumnNames(columnNames)
, fTableName(tableName)
{}

ASelectFromTableCommand ASelectFromTableCommand::Distinct()
{
   fDistinct = true;
   return *this;
}

ASelectFromTableCommand ASelectFromTableCommand::Join(const std::string& tableName,
                                                      const std::string& condition)
{
   fStatement.emplace_back(kJoin, tableName);
   fStatement.emplace_back(kJoinOn, condition);
   return *this;
}

void ASelectFromTableCommand::SetPageSize(size_t pageSize)
{
   fPageSize = pageSize;
}


void ASelectFromTableCommand::SetPage(size_t page)
{
   fPage = page;
}

ASelectFromTableCommand ASelectFromTableCommand::As(const std::string& columnName)
{
   fStatement.emplace_back(kAs, columnName);
   return *this;
}

std::string ASelectFromTableCommand::AsSql() const
{
   std::string cmd;
   cmd.reserve(this->EstimateStatementLength());
   
   cmd += "SELECT";
   
   if (fDistinct)
   {
      cmd += " DISTINCT";
   }
   
   const char* separator = " ";
   for (const auto& name : fColumnNames)
   {
      SBuilder::Build(cmd, separator, name);
      separator = ", ";
   }
   cmd += " FROM " + fTableName;
   
   for (const auto& clause : fStatement)
   {
      auto type = clause.first;
      auto expr = clause.second;
      switch (type)
      {
         case kAnd:
         SBuilder::Build(cmd, " AND ", expr);
         break;
            
         case kAs:
         SBuilder::Build(cmd, " AS ", expr);
         break;
            
         case kJoin:
         SBuilder::Build(cmd, " JOIN ", expr);
         break;
            
         case kJoinOn:
         SBuilder::Build(cmd, " ON ", expr);
         break;
            
         case kIn:
         SBuilder::Build(cmd, " IN ", expr);
         break;

         case kWhere:
         SBuilder::Build(cmd, " WHERE ", expr);
         break;

         case kOrderBy:
         SBuilder::Build(cmd, " ORDER BY ", expr);
         break;
      }
   }
 
   if (kNoPaging != fPageSize)
   {
      // TODO: OFFSET and LIMIT
   }
   
   cmd += ";";
   return cmd;
}

size_t ASelectFromTableCommand::EstimateStatementLength() const
{
   // Factor tries to account for the keywords, separators and spaces.
   return 1.15 * std::accumulate(fStatement.begin(), fStatement.end(),
                                fStatement.size() * 4,
    [](size_t current, const auto& clausePair)
    {
      return current + clausePair.second.size();
    });
}

ASelectFromTableCommand ASelectFromTableCommand::OrderBy(const std::vector<std::string>& columnNames)
{
   auto orderingTerms = std::string{};
   const char* separator = "";
   for (const auto& term : columnNames)
   {
      SBuilder::Build(orderingTerms, separator, term);
      separator = ", ";
   }
   orderingTerms += " ";

   fStatement.emplace_back(kOrderBy, orderingTerms);

   return *this;
}


ASelectFromTableCommand ASelectFromTableCommand::Where(const std::string& condition)
{
   // Add additional space at end so multiple Where calls can be made.
   fStatement.emplace_back(kWhere, condition + " ");
   return *this;
}

ASelectFromTableCommand ASelectFromTableCommand::WhereEqual(const std::string& columnName,
                                                            const std::string& value)
{
   // Add additional space at end so multiple Where calls can be made.
   this->Where(AEquals(columnName, value));
   return *this;
}

ASelectFromTableCommand ASelectFromTableCommand::WhereIn(const std::string& columnName,
                                                         const std::vector<std::string>& values)
{
   fStatement.emplace_back(kWhere, columnName);
   auto inClause = std::string(" (");
   const char* separator = "";
   for (const auto& value : values)
   {
      SBuilder::Build(inClause, separator, value);
      separator = ", ";
   }
   inClause += ")";
   fStatement.emplace_back(kIn, inClause);

   return *this;
}

ASelectFromTableCommand ASelectFromTableCommand::AndEqual(const std::string& columnName,
                                                          const std::string& value)
{
   // TODO: Check that fStatement contains a WHERE clause, else this is an error.
   // Add additional space at end so multiple Where calls can be made.
   fStatement.emplace_back(kAnd, (std::string)AEquals(columnName, value) + " ");
   return *this;
}


// MARK: -

AUpdateCommand::AUpdateCommand(const std::string& tableName,
                               const std::string& key,
                               const std::string& columnName,
                               const std::string& value)
: fTableName{tableName}
, fKey{key}
, fColumnName{columnName}
, fValue{value}
{}

std::string AUpdateCommand::AsSql() const
{
   std::string cmd;
   SBuilder::Build(cmd, "UPDATE ", fTableName, " SET ", fColumnName, " = ", fValue,
                   " WHERE rowId = ", fKey);
   return cmd;
}


//*****************************************************************************
// MARK: - Transaction support

std::string ABeginTransaction::AsSql() const
{
   return "BEGIN TRANSACTION;";
}

std::string AEndTransaction::AsSql() const
{
   return "COMMIT TRANSACTION;";
}

std::string ARollbackTransaction::AsSql() const
{
   return "ROLLBACK;";
}


// ****************************************************************************
// MARK: - Subexpressions


AConditional::AConditional(const std::string& lhs, const std::string& op,
                           const std::string& rhs)
: fLhs(lhs), fOp(op), fRhs(rhs)
{}

std::string AConditional::AsSql() const
{
   auto condition = std::string();
   SBuilder::Build(condition, fLhs, fOp, fRhs);
   return condition;
}

AConditional AConditional::And(const AConditional& rhs) const
{
   return AAnd{*this, rhs};
}


AAnd::AAnd(const std::string& leftSide, const std::string& rightSide)
: AConditional(leftSide, " AND ", rightSide)
{}


AEquals::AEquals(const std::string& leftSide, const std::string& rightSide)
: AConditional(leftSide, "=", rightSide)
{}


AIn::AIn(const std::string& columnName, const ValuesSet& values)
: AConditional(columnName, " IN (", StringUtils::Join(values, ",") += ")")
{}

AIn::AIn(const std::string& columnName, const Keys& values)
: AConditional(columnName, " IN (", StringUtils::Join(values, ",") += ")")
{}


// MARK: -

AQualifiedKey::AQualifiedKey(const std::string& qualifier, const std::string& fieldName)
: fQualifier(qualifier)
, fFieldName(fieldName)
{}

std::string AQualifiedKey::AsSql() const
{
   std::string cmd;
   SBuilder::Build(cmd, fQualifier, ".", fFieldName);
   return cmd;
}

ATableKey::ATableKey(const std::string& tableName, const std::string& fieldName)
: AQualifiedKey(tableName, fieldName)
{}

ASchemaOld::ASchemaOld(const std::string& fieldName)
: AQualifiedKey("OLD", fieldName)
{}

ASchemaNew::ASchemaNew(const std::string& fieldName)
: AQualifiedKey("NEW", fieldName)
{}


// MARK: -

AUniqueConstraint::AUniqueConstraint(const ColumnNames& columnNames)
: fColumns(columnNames)
{}

AUniqueConstraint::AUniqueConstraint(const ColumnNames&& columnNames)
: fColumns(columnNames)
{}

std::string AUniqueConstraint::AsSql() const
{
   std::string cmd;
   cmd.reserve(7 + 2);
   cmd = "UNIQUE(";
   std::string separator = "";
   for (const auto& name : fColumns)
   {
      cmd += separator;
      cmd += name;
      separator = ", ";
   }
   // add a space to prevent a type being added to this when used as a column value
   // in another ADatabaseCommand
   cmd += ") ";
   return cmd;
}

// MARK: -

AColumn::AColumn(const std::string& name, const std::string& type/*="TEXT"*/)
: fName(name)
, fType(type)
, fUnique(false)
{}

std::string AColumn::AsSql() const
{
   std::string cmd;
   SBuilder::Build(cmd, fName, " ", fType, fUnique ? " UNIQUE" : "");
   return cmd;
}

AColumn AColumn::Unique()
{
   fUnique = true;
   return *this;
}


// MARK: -

ASubqueryValue::ASubqueryValue(const ADatabaseCommand& query)
{
   SBuilder::Build(fStatement, "(", (std::string)query, ")");
}


// MARK: - Internal

namespace
{
   int AppendResultMap(void* commandContext, int columnCount,
                        char** values, char** names)
   {
      assert(nullptr != commandContext);
      RowResult rowResult;
      for (int c = 0; c < columnCount; ++c)
      {
         const char* const val = *(values + c);
         const char* const name = *(names + c);
         if (nullptr != name)
         {
            rowResult[name] = (nullptr != val ? val : "NULL");
         }
      }
      ADatabaseCommand* command = reinterpret_cast<ADatabaseCommand*>(commandContext);
      command->AddRowResult(std::move(rowResult));
      
      return 0;
   }
}
