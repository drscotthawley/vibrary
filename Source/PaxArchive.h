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

#include <archive.h> // Using libarchive
#include <archive_entry.h>

#include "../JuceLibraryCode/JuceHeader.h"

/**
 * Abstract base class to manage simple operations on an archive.
 */
class APaxArchive
{
public:
   explicit APaxArchive(const File& file) noexcept;
   virtual ~APaxArchive();

   void Open();
   void Close();

   [[nodiscard]] constexpr bool IsOpen() const noexcept { return fOpen; };

   /** Adds appropriate extensions to baseName */
   static String MakeArchiveName(const String& baseName);


   APaxArchive(const APaxArchive&) = delete;
   APaxArchive& operator=(const APaxArchive&) = delete;

protected:

   /**
    * Open the archive as appropreat for subclass' purpose.
    * \return libarchive result code.
    */
   virtual int OpenArchive() = 0;

   /**
    * Close the archive as appropriate for subclass' purpose
    * \return libarchive result code.
    */
   virtual int CloseArchive() = 0;

   /**
    * Allocate the archive object as appropriate for subclass' purpose
    * \return the new archive objet
    */
   virtual struct archive* NewArchive() = 0;

   /**
    * Free the archive object as appropriate for subclass' purpose
    * \return libarchive result code
    */
   virtual int FreeArchive() = 0;

   /**
    * If result does not indicate an error, nothing is done.
    * Otherwise, logs message with some additional information such as the error string.
    * For serious errors (`result < ARCHIVE_WARN`) throws a std::runtime_error
    * with the amended message.
    * \result An result code from a libarchive function
    * \message A simple message about the operation that returned the error.
    */
   void HandleErrors(int result, const std::string& message) const;

   File fFile;

   // Could make this private and pass it to the above API funcs.
   struct archive* fPax;

private:
   bool fOpen;
};

/**
 * Manages writing a tar gzipped archive in pax format.
 *
 * Use the convenience method MakeFrom() to easily add a directory and its
 * contents to an archive.
 */
class APaxMaker final : public APaxArchive
{
public:

   /**
    * \returns a file pointing to a gzipped tar archive with the contents of rootDir.
    */
   static File MakeFrom(const File& rootDir) noexcept(false);
   
   explicit APaxMaker(const File& filename);
   ~APaxMaker();

   /** \returns The file containing the archive. */
   operator File() const;

   void AddDirectoryRecursively(const File& rootDir);
   void AddFile(const File& file);

   APaxMaker(const APaxMaker&) = delete;
   APaxMaker& operator=(const APaxMaker&) = delete;
   
private:

   int OpenArchive() override;
   int CloseArchive() override;
   struct archive* NewArchive() override;
   int FreeArchive() override;

   struct AStat
   {
      AStat() {}
      AStat(const File& file); //!< inefficiently set members from file's stats
        
      bool isDirectory;
      bool isHidden;
      int64 fileSize;
      Time modTime;
      Time creationTime;
      bool isReadOnly;
   };
   
   void AddFile(const File& file, const AStat& stat);
   void WriteEntry(const File& file, const AStat& stat);
   void WriteFileContents(const File& file);
   
   std::string TrimRelativePath(const File& file) const;

   std::string fRelativePath;
   struct archive_entry* fEntry;
};


/**
 * Manages extracting files from a gzipped tar archive in pax format.
 */
class APaxExtractor final : public APaxArchive
{
public:

   /** Extract the archive contents into outDir. */
   static void ExtractAll(const File& archive, const File& outDir) noexcept(false);

   explicit APaxExtractor(const File& archive) noexcept(false);
   ~APaxExtractor();

   /** Extract all files in the archive, placing them in outDir */
   void ExtractAllTo(const File& outDir);

   struct archive_entry* Entry() const;

private:

   int OpenArchive() override;
   int CloseArchive() override;
   struct archive* NewArchive() override;
   int FreeArchive() override;

   /** Manage a FILE*. Opens the file for reading, and closes in dtor. */
   struct FilePtr
   {
      FilePtr(const File& file);
      ~FilePtr();
      [[nodiscard]] constexpr operator FILE*() const { return fd; }
   private:
      FILE* fd;
   } fFilePtr;

   struct archive_entry* fEntry;
};

