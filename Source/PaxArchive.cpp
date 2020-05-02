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

#include <cerrno>
#include <cstdio>
#include <stdexcept>

#if __MACH__
#include <mach/i386/vm_param.h>
#endif

#include "PaxArchive.h"


namespace
{
   time_t ToTime_t(const Time& time)
   {
      return time.toMilliseconds() / 1000L;
   }

   constexpr bool IsOk(int result)
   {
      return ARCHIVE_OK <= result;
   }

   constexpr bool IsSeriousError(int result)
   {
      return ARCHIVE_WARN > result;
   }

   constexpr bool IsEof(int result)
   {
      return ARCHIVE_EOF == result;
   }

   /**
    * Manages writing files to disk.
    * For use by APaxExtractor extraction functions.
    */
   class ADiskFileWriter final : public APaxArchive
   {
   public:
      /**
       * Automatically opens the extractor and closes it in the destructor.
       *
       * Changes the current working directory to outDir, and restores the previous
       * working directory when this is destructed. This cwd change is required by
       * how libarchive works. Using this object to manage it is not (just) cleverness.
       *
       * The previous working directory is stored in fFile.
       */
      ADiskFileWriter(const File& outDir) noexcept(false);
      ~ADiskFileWriter();

      /** Any existing files extracted are overwritten. */
      int ExtractFile(struct archive* source, struct archive_entry* entry) noexcept;

   private:

      int CopyData(struct archive* source) const noexcept;

      int OpenArchive();
      int CloseArchive();
      struct archive* NewArchive();
      int FreeArchive();
   };
}



String APaxArchive::MakeArchiveName(const String& baseName)
{
   return baseName + ".tar.gz";
}



APaxArchive::APaxArchive(const File& file) noexcept
: fFile(file)
, fPax(nullptr)
, fOpen(false)
{}


APaxArchive::~APaxArchive()
{
   // Eat any exceptions.
   if (fOpen)
   {
      try
      {
         this->Close();
      }
      catch (const std::exception& ex)
      {
         std::cerr << "Excetion while freeing an APaxArchive.\n\t" << ex.what() << "\n";
      }
      catch (...)
      {
         std::cerr << "Unknown exception while freeing an APaxArchive.\n";
      }
   }
}


void APaxArchive::Close()
{
   if (fOpen)
   {
      this->HandleErrors(this->CloseArchive(), "Failed to close the archive");
      fOpen = false;
   }

   if (nullptr != fPax)
   {
      this->HandleErrors(this->FreeArchive(), "Failed to finish writing the archive");
      fPax = nullptr;
   }
}


void APaxArchive::HandleErrors(int result, const std::string& message) const
{
   if (!IsOk(result))
   {
      std::stringstream elaborated;
      elaborated << message;
      auto errorMsg = archive_error_string(fPax);
      if (errorMsg)
      {
         elaborated << ": " << errorMsg;
      }
      std::cerr << elaborated.str() << "\n";

      if (IsSeriousError(result))
      {
         throw std::runtime_error(elaborated.str());
      }
   }
}


void APaxArchive::Open()
{
   fPax = this->NewArchive();
   if (nullptr == fPax)
   {
      throw std::runtime_error("Failed to allocate the archive.");
   }

   const int result = this->OpenArchive();
   this->HandleErrors(result, "Failed to open archive");

   fOpen = IsOk(result);
}


//MARK: - APaxMaker


File APaxMaker::MakeFrom(const File& rootDir) noexcept(false)
{
   File result;
   try
   {
      const auto outname = MakeArchiveName(rootDir.getFileNameWithoutExtension());
      const auto out = rootDir.getParentDirectory().getChildFile(outname);

      APaxMaker pax(out);
      pax.Open();
      pax.AddDirectoryRecursively(rootDir);
      pax.Close();
      result = pax;
   }
   catch (const std::runtime_error& ex)
   {
      std::cerr << "Failed to archive " << rootDir.getFileName().toStdString()
       << ": " << ex.what() << "\n";
      throw ex;
   }
   return result;
}


APaxMaker::APaxMaker(const File& file)
: APaxArchive(file)
, fRelativePath(file.getParentDirectory().getFullPathName().toStdString())
, fEntry(archive_entry_new())
{}


APaxMaker::~APaxMaker()
{
   try
   {
      this->Close();
   }
   catch (...)
   {
      // eat it
   }

   if (nullptr != fEntry)
   {
      archive_entry_free(fEntry);
      fEntry = nullptr;
   }
}


APaxMaker::operator File() const
{
    return fFile;
}


void APaxMaker::AddDirectoryRecursively(const File& rootDir)
{
   DirectoryIterator dirIter(rootDir, false, "*", File::findFilesAndDirectories);
   AStat stat;

   // Depth first walk of the directory tree.
   while (dirIter.next(&stat.isDirectory, &stat.isHidden, &stat.fileSize,
                       &stat.modTime, &stat.creationTime, &stat.isReadOnly))
   {
      if (stat.isDirectory)
      {
         this->AddDirectoryRecursively(dirIter.getFile());
      }
      else
      {
         this->AddFile(dirIter.getFile(), stat);
      }
   }
}


void APaxMaker::AddFile(const File& file) noexcept(false)
{
   if (!this->IsOpen())
   {
      throw std::runtime_error("Cannot add a file to an archive that is not open.");
   }

   this->AddFile(file, AStat(file));
}


void APaxMaker::AddFile(const File& file, const AStat& stat)
{
   if (!this->IsOpen())
   {
      throw std::runtime_error("Cannot add a file to an archive that is not open.");
   }

   this->WriteEntry(file, stat);
   if (!stat.isDirectory)
   {
      this->WriteFileContents(file);
   }
}


int APaxMaker::CloseArchive()
{
   return archive_write_close(fPax);
}


int APaxMaker::FreeArchive()
{
   return archive_write_free(fPax);
}


struct archive* APaxMaker::NewArchive()
{
   return archive_write_new();
}


int APaxMaker::OpenArchive()
{
   archive_write_add_filter_gzip(fPax);
   archive_write_set_format_pax_restricted(fPax);

   return archive_write_open_filename(fPax,
    fFile.getFullPathName().toStdString().c_str());
}


std::string APaxMaker::TrimRelativePath(const File& file) const
{
   std::string filename = file.getFullPathName().toStdString();
   auto start = filename.find(fRelativePath);
   if (std::string::npos != start)
   {
      // TODO: More platform agnostic way to move past the path separator.
      filename = filename.substr(start + fRelativePath.size() + 1);
   }
   return filename;
}


void APaxMaker::WriteEntry(const File& file, const AStat& stat)
{
   if (stat.isDirectory)
   {
      return;
   }

   archive_entry_clear(fEntry);

   std::string filename = this->TrimRelativePath(file);

   archive_entry_set_pathname(fEntry, filename.c_str());
   archive_entry_set_filetype(fEntry, AE_IFREG);
   archive_entry_set_perm(fEntry, stat.isReadOnly ? 0444 : 0644);
   archive_entry_set_ctime(fEntry, ToTime_t(stat.creationTime), 0); // not sure what the 0 is
   archive_entry_set_mtime(fEntry, ToTime_t(stat.modTime),      0);
   archive_entry_set_size(fEntry, stat.fileSize); // Note 3

   int result = archive_write_header(fPax, fEntry);
   this->HandleErrors(result, "Failed to write entry header");
}


void APaxMaker::WriteFileContents(const File& file)
{
   FileInputStream input(file);
   if (input.openedOk())
   {
#ifdef PAGE_SIZE
      uint8_t buff[PAGE_SIZE];
#else
      uint8_t buff[8192];
#endif
      while (!input.isExhausted())
      {
         int numRead = input.read(buff, sizeof(buff));
         auto numWritten = archive_write_data(fPax, buff, numRead);
         if (numRead != numWritten)
         {
            std::cerr << "Warning: failed to write requested number of bytes to archive: "
                      << archive_error_string(fPax) << "\n";
         }
      }
   }
}


// MARK: - AStat

// Juce docs suggest this isn't an efficient way to get the file's stats.
// But it doesn't reveal any way to do it more efficiently without a
// DirectoryIterator.
APaxMaker::AStat::AStat(const File& file)
: isDirectory(file.isDirectory())
, isHidden(file.isHidden())
, fileSize(file.getSize())
, modTime(file.getLastModificationTime())
, creationTime(file.getCreationTime())
, isReadOnly(!file.hasWriteAccess())
{}


// MARK - APaxExtractor

void APaxExtractor::ExtractAll(const File& archive, const File& outDir) noexcept(false)
{
   try
   {
      APaxExtractor pax(archive);
      pax.Open();
      pax.ExtractAllTo(outDir);
      pax.Close();
   }
   catch (const std::runtime_error& ex)
   {
      std::cerr << "Failed to unarchive " << archive.getFileName()
       << ": " << ex.what() << "\n";
      throw;
   }
}


APaxExtractor::APaxExtractor(const File& archive)
: APaxArchive(archive)
, fFilePtr(archive)
{}


APaxExtractor::~APaxExtractor()
{
   this->Close();
}


int APaxExtractor::CloseArchive()
{
   return archive_read_close(fPax);
}


void APaxExtractor::ExtractAllTo(const File& outDir)
{
   std::cerr << "Extracting to " << outDir.getFullPathName() << "\n";

   ADiskFileWriter writer(outDir);

   int result = ARCHIVE_OK;
   while (IsOk(result))
   {
      result = archive_read_next_header(fPax, &fEntry);
      if (IsEof(result))
      {
         break;
      }
      else
      {
         this->HandleErrors(result, "Failed to extract next file.");
      }

      std::cerr << "Extracting " << archive_entry_pathname(fEntry) << "\n";

      result = writer.ExtractFile(fPax, fEntry);
   }
   std::cerr << "Archive extraction complete\n";
}


int APaxExtractor::FreeArchive()
{
   return archive_read_free(fPax);
}


struct archive* APaxExtractor::NewArchive()
{
   return archive_read_new();
}


int APaxExtractor::OpenArchive()
{
   archive_read_support_filter_gzip(fPax);
   archive_read_support_format_all(fPax);

   return archive_read_open_FILE(fPax, fFilePtr);
}


// MARK: - APaxExtractor::FileDesc

APaxExtractor::FilePtr::FilePtr(const File& file)
: fd(std::fopen(file.getFullPathName().toStdString().c_str(), "r+"))
{
   if (nullptr == fd)
   {
      throw std::runtime_error(std::strerror(errno));
   }
}

APaxExtractor::FilePtr::~FilePtr()
{
   std::fclose(fd);
   fd = nullptr;
}


namespace
{
   // MARK: - ADiskFileWriter

   ADiskFileWriter::ADiskFileWriter(const File& outDir)
   : APaxArchive(File::getCurrentWorkingDirectory())
   {
      outDir.setAsCurrentWorkingDirectory();
      this->Open();
   }


   ADiskFileWriter::~ADiskFileWriter()
   {
      try
      {
         fFile.setAsCurrentWorkingDirectory();
         this->Close();
      }
      catch (...)
      {
         std::cerr << "Failed to close APaxExtractor.\n";
      }
   }


   int ADiskFileWriter::CloseArchive()
   {
      return archive_write_close(fPax);
   }


   int ADiskFileWriter::CopyData(struct archive* source) const noexcept
   {
      int result = ARCHIVE_OK;
      const void *buff = nullptr;
      size_t size = 0;
      la_int64_t offset = 0;

      while (IsOk(result))
      {
         result = archive_read_data_block(source, &buff, &size, &offset);
         if (IsEof(result) || !IsOk(result))
         {
            break;
         }

         const ssize_t written = archive_write_data_block(fPax, buff, size, offset);
         result = static_cast<int>(written);
      }

      return IsEof(result) ? ARCHIVE_OK : result;
   }


   int ADiskFileWriter::ExtractFile(struct archive* source, struct archive_entry* entry) noexcept
   {
      int result = archive_write_header(fPax, entry);
      if (IsOk(result) && archive_entry_size(entry) > 0)
      {
         result = this->CopyData(source);
      }
      if (IsOk(result))
      {
         result = archive_write_finish_entry(fPax);
      }
      return result;
   }


   int ADiskFileWriter::FreeArchive()
   {
      return archive_write_free(fPax);
   }


   struct archive* ADiskFileWriter::NewArchive()
   {
      return archive_write_disk_new();
   }


   int ADiskFileWriter::OpenArchive()
   {
      constexpr int kFlags = ARCHIVE_EXTRACT_SECURE_NOABSOLUTEPATHS
       | ARCHIVE_EXTRACT_TIME
       | ARCHIVE_EXTRACT_PERM
       | ARCHIVE_EXTRACT_ACL
       | ARCHIVE_EXTRACT_FFLAGS
       | ARCHIVE_EXTRACT_MAC_METADATA
       ;

      int result = archive_write_disk_set_options(fPax, kFlags);
      if (ARCHIVE_OK == result)
      {
         result = archive_write_disk_set_standard_lookup(fPax);
      }
      return result;
   }

}
