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

#include <string>
#include <limits>
#include <unordered_map>
#include <vector>

#include "DatabaseCallback.h"

class ADatabase;
class AColumn;

using RowResult = std::unordered_map<std::string, std::string>;//!< Maps column name to value
using CommandResults = std::vector<RowResult>;

/**
 * \file Commands.h
 * \brief Commands for interacting with a database.
 *
 * The design philosophy is to remain database and language agnostic but
 * SQL is the only supported query language and some short cuts have been taken
 * within the implementation in lieue of true language independence.
 */


/**
 * Interface to database commands.
 */
class ADatabaseCommand
{
public:
   virtual ~ADatabaseCommand();
   
   virtual const CommandResults& Results() const;
   virtual const RowResult& Result(size_t rowIndex) const;
   
   virtual std::string AsSql() const = 0;
   
   /** \return The AsSql() result removing the last character, assummed to be ';' */
   virtual operator std::string() const;
   
   /** \return Default callback calls AddRowResult with each one row's result */
   virtual Database::Callback ResultCallback() const;

   // For use by ResultCallback
   virtual void AddRowResult(const RowResult& result);
   virtual void AddRowResult(const RowResult&& result);
   
private:
   friend class ADatabase;
   CommandResults fResults;
};


class ACreateTableCommand final : public ADatabaseCommand
{
public:
   using ColumnNames = std::vector<std::string>;
   using Target = std::pair<std::string,std::string>;
   using Relations = std::unordered_map<std::string,Target>; // column -> (table,column)
   
   ACreateTableCommand(const std::string& tableName);
 
   void AddColumn(const AColumn& column);
   void AddColumn(const AColumn&& column);
   /** \param name A column name or a column name and a SQL type, e.g "name TEXT" */
   void AddColumn(const std::string& name);
   void AddColumn(const std::string&& name);
   void AddColumns(const ColumnNames& names);
   void AddColumns(const ColumnNames&& names);
   
   void AddRelation(const std::string& name, const std::string& toTable,
                    const std::string& targetColumn);
   
   std::string AsSql() const override;

private:
   size_t EstimateCommandSize() const;
   
   std::string fTableName;
   ColumnNames fColumns;
   Relations fRelationships;
};


/** Support types for CreateTriggerCommand */
namespace Trigger
{
   enum Aspect { kBefore, kAfter, kInstead };
   enum Command { kDelete, kInsert, kUpdate };

   struct IsTemp
   {
     IsTemp(bool isTemp = true) : fTemp(isTemp) {}
     constexpr operator bool() const noexcept { return fTemp; }
     const bool fTemp;
   };

   struct ForEachRow
   {
     ForEachRow(bool forEach = true) : fDo(forEach) {}
     constexpr operator bool() const noexcept { return fDo; }
     const bool fDo;
   };
}


class ACreateTriggerCommand final : public ADatabaseCommand
{
public:
   ACreateTriggerCommand(const std::string& name, const std::string& onTable,
                         Trigger::Aspect aspect, Trigger::Command triggerByCommand,
                         Trigger::IsTemp temporary, Trigger::ForEachRow doForEachRow,
                         const std::vector<std::string>& triggeredCommands);

   ACreateTriggerCommand When(const std::string& condition);

   std::string AsSql() const override;

private:
   // These will always be in this order in fStatement, with kWhen being optional.
   enum Clause { kAspect, kTrigByCommand, /* for update kOfColumn,*/ kWhen };
   using Statement = std::vector<std::pair<Clause, std::string>>;

   const char* AspectName(Trigger::Aspect aspect) const;
   const char* TriggerCommandName(Trigger::Command triggerByCommand);

   Statement fStatement;
   std::string fName;
   std::string fTableName;
   std::string fCommands;
   bool fTemporary = false;
   bool fForEachRow = false;
};


class ADeleteFromTable final : public ADatabaseCommand
{
public:
   ADeleteFromTable(const std::string& table,
                    const std::string& whereCondition = std::string());

   std::string AsSql() const override;
private:
   const std::string fTableName;
   const std::string fCondition;
};

class AInsertCommand final : public ADatabaseCommand
{
public:
   using Strings = std::vector<std::string>;
   
   /** Init with values in column order. */
   AInsertCommand(const std::string& table, const Strings& values);
   /** Init with values in order of columns argument, each column corresponding to a value. */
   AInsertCommand(const std::string& table, const Strings& values, const Strings& columns);
   
   std::string AsSql() const override;
   
private:
   size_t EstimateCommandSize() const;
   
   std::string fTableName;
   Strings fColumns;
   Strings fValues;
};


class APragmaCommand final : public ADatabaseCommand
{
public:
   APragmaCommand(const std::string& name, const std::string& value);
   std::string AsSql() const override;
private:
   std::string fCommand;
};



// MARK: -

class ASelectFromTableCommand final : public ADatabaseCommand
{
public:
   using ColumnNames = std::vector<std::string>;
   
   ASelectFromTableCommand(const std::string& tableName);
   ASelectFromTableCommand(const std::string& tableName, const ColumnNames& columnNames);
   
   ASelectFromTableCommand Distinct();
   ASelectFromTableCommand As(const std::string& columnName);

   ASelectFromTableCommand Where(const std::string& condition);

   // $$$: Could simplify the Where family and make caller always create the
   // full expresssion passed to Where. Especially AndEqual which is prone to user error.

   ASelectFromTableCommand WhereEqual(const std::string& columnName, const std::string& value);

   /** Add to a kWhere clause with a condition that must also be met */
   [[deprecated]]
   ASelectFromTableCommand AndEqual(const std::string& columnName,
                                    const std::string& value);
   
   ASelectFromTableCommand WhereIn(const std::string& columnName,
                                   const std::vector<std::string>& values);
   
   ASelectFromTableCommand Join(const std::string& tableName,
                                const std::string& onCondition);

   // TODO: Add ordering direction
   ASelectFromTableCommand OrderBy(const std::vector<std::string>& columnNames);
   
   constexpr static size_t kNoPaging = std::numeric_limits<size_t>::max();
   /** Limit number of values returned. Set to kNoPaging to return all results. */
   void SetPageSize(size_t pageSize);
   void SetPage(size_t page);
   
   std::string AsSql() const override;
   
private:
   size_t EstimateStatementLength() const;
   
   enum Clause { kAs, kJoin, kJoinOn, kIn, kWhere, kAnd/*deprecated*/, kOrderBy };
   using Statement = std::vector<std::pair<Clause, std::string>>;
   
   Statement fStatement;
   ColumnNames fColumnNames;
   const std::string fTableName;
   size_t fPageSize = kNoPaging;
   size_t fPage = 0;
   bool fDistinct = false;
};


class AUpdateCommand final : public ADatabaseCommand
{
public:
   AUpdateCommand(const std::string& tableName, const std::string& key,
                  const std::string& columnName,
                  const std::string& value);

   std::string AsSql() const override;

private:
   std::string fTableName;
   std::string fKey;
   std::string fColumnName;
   std::string fValue;
};


// MARK: - Subexpressions

/**
 * Subexpressions can go within other commands but not stand on their own.
 *
 * It differs from ADatabaseCommand by never appending the final character,
 * (expected to be ';' in the command's SQL rendering.)
 * $$$: A better way might be to define a Terminate() method on ADatabaseCommand
 * then call or not call it.
 */
class ASubexpression
{
public:
   virtual ~ASubexpression()
   {}

   virtual std::string AsSql() const = 0;
   virtual operator std::string() const { return this->AsSql(); }
};


/**
 * A literal value to use in a command. It handles quoting the value
 * and converting the value to a string.
 */
template <typename T>
class AValue final : public ASubexpression
{
public:
   explicit AValue(const T& value) : fValue(value)
   {}
   std::string AsSql() const override { return *this; }
   operator std::string() const override { return Quoted(); }
private:
   inline std::string Quoted() const { return "'" + AsString() + "'"; }
   inline std::string AsString() const { return std::to_string(fValue); }
   const T fValue;
};
template<>
inline std::string AValue<std::string>::AsString() const { return fValue; }

/**
 * A subquery result expected to be inserted in a list of values. I.e. it needs
 * to be in parentheses.
 */
class ASubqueryValue final : public ASubexpression
{
public:
   explicit ASubqueryValue(const ADatabaseCommand& query);
   /** The command passed to the ctor contained in parentheses. */
   operator std::string() const override { return fStatement; }
   std::string AsSql() const override { return *this; }
private:
   std::string fStatement;
};


/** Define a column in the table with a default type of TEXT. */
class AColumn final : public ASubexpression
{
public:
   AColumn(const std::string& name, const std::string& type = "TEXT");
   std::string AsSql() const override;
   AColumn Unique();
protected:
   std::string fName;
   std::string fType;
   bool fUnique;
};


/** Table level unique contraint for one or more columns */
class AUniqueConstraint final : public ASubexpression
{
public:
   using ColumnNames = std::vector<std::string>;
   AUniqueConstraint(const ColumnNames& columnNames);
   AUniqueConstraint(const ColumnNames&& columnNames);
   std::string AsSql() const override;
private:
   const ColumnNames fColumns;
};


// MARK: -

/**
 * Support for qualifying keys with table names or schemas.
 */
class AQualifiedKey : public ASubexpression
{
public:
   AQualifiedKey(const std::string& qualifier, const std::string& fieldName);
   std::string AsSql() const override;
protected:
   const std::string fQualifier;
   const std::string fFieldName;
};

/**
 * Produces a qualified key for cases when two tables in the same command
 * have the same key.
 * (e.g.) select table1.name,table2.name table1 left join table2...
 *
 * In SQL produces 'tableName.fieldName'
 */
class ATableKey final : public AQualifiedKey
{
public:
   ATableKey(const std::string& tableName, const std::string& fieldName);
};

/**
 * Produces a OLD schema qualified name, e.g. old.
 */
class ASchemaOld final : public AQualifiedKey
{
public:
   ASchemaOld(const std::string& fieldName);
};

/**
 * Produces a NEW schema qualified name
 */
class ASchemaNew final : public AQualifiedKey
{
public:
   ASchemaNew(const std::string& fieldName);
};


// MARK: -

/**
 * Binary relational operator parent class
 */
class AConditional : public ASubexpression
{
public:
   AConditional(const std::string& leftSide, const std::string& op,
                const std::string& rightSide);

   AConditional And(const AConditional& rightSide) const;

   std::string AsSql() const override;
private:
   const std::string fLhs;
   const std::string fOp;
   const std::string fRhs;
};

class AEquals final : public AConditional
{
public:
   AEquals(const std::string& leftSide, const std::string& rightSide);
};


class AAnd final : public AConditional
{
public:
   AAnd(const std::string& leftSize, const std::string& rightSide);
};


class AIn final : public AConditional
{
public:
   using ValuesSet = std::vector<std::string>;
   using Keys = std::vector<int64_t>;
   AIn(const std::string& columnName, const ValuesSet& values);
   AIn(const std::string& columnName, const Keys& keys);
private:
};


// MARK: -
/** See ATransaction in Database.h */

class ABeginTransaction final : public ADatabaseCommand
{
public:
   std::string AsSql() const override;
};

/** See ATransaction */
class AEndTransaction final : public ADatabaseCommand
{
public:
   std::string AsSql() const override;
};

/** See ATransaction */
class ARollbackTransaction final : public ADatabaseCommand
{
public:
   std::string AsSql() const override;
};

