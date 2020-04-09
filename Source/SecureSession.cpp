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

#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

#include "SecureSession.h"
#include "StringUtils.h"


using UniqueScpPtr = std::unique_ptr<ssh_scp_struct, void(*)(ssh_scp)>;


namespace
{
   void EndScpSession(ssh_scp scp);
   void ReportError(ssh_session session, const char* const message, int result);
}

// MARK: - KeyHash

/**  Manage libSSH hash memory, minimally. */
struct KeyHash
{
   unsigned char* fHash;
   size_t fLength;

   KeyHash() : fHash(nullptr), fLength(0UL)
   {}

   ~KeyHash()
   {
      if (nullptr != fHash)
         ssh_clean_pubkey_hash(&fHash);
   }

   void Print()
   {
      std::unique_ptr<char, void(*)(char*)>
       hexa(ssh_get_hexa(fHash, fLength), ssh_string_free_char);
      std::cout << "Public key hash " << hexa << "\n";
   }

   // fHash is managed so prevent copying.
   KeyHash(const KeyHash&)            = delete;
   KeyHash& operator=(const KeyHash&) = delete;
};


// MARK: - ASecureSession

constexpr int ASecureSession::kDefaultPort;


ASecureSession::ASecureSession(const std::string& user, const std::string& host, int port/*=22*/,
                               const std::string& configFilename/*=""*/,
                               TrustServerFunc trustFunc/*=nullptr*/) noexcept(false)
: fConfigFilename(configFilename)
, fTrustServerFunc(trustFunc)
{
   if (SSH_OK != this->Init(user, host, port))
   {
      throw std::runtime_error(ssh_get_error(fSession.getCSession()));
   }
}


ASecureSession::~ASecureSession()
{
   try
   {
      this->CleanUp();
   }
   catch (...)
   {
      std::cerr << "Unknown exception what cleaning up for ASecureSession.\n";
      // eat it
   }
}


int ASecureSession::Init(const std::string& user, const std::string& host, int port)
{
   this->SetOptions(user, host, port, SSH_LOG_WARNING);
   
   fSession.connect();
   int result = this->VerifyKnownHost();
   if (SSH_OK != result)
   {
      fSession.disconnect();
      throw std::runtime_error("Training canceled.");
   }
   // TODO: Try different auth methods until success or no more methods
   return fSession.userauthPublickeyAuto(); // may throw
}


int ASecureSession::CleanUp()
{
   fSession.disconnect();
   return SSH_OK;
}


int ASecureSession::HashServerPublicKey(KeyHash& keyHash)
{
   auto csession = fSession.getCSession();
   
   ssh_key serverPubKey;
   ssh_get_server_publickey(csession, &serverPubKey);
   std::cerr << "Key type: " << ssh_key_type_to_char(ssh_key_type(serverPubKey)) << "\n";
   
   unsigned char* hash = nullptr;
   size_t hashLength = 0UL;
   int result = ssh_get_publickey_hash(serverPubKey, SSH_PUBLICKEY_HASH_MD5,
                                       &hash, &hashLength);
   
   ssh_key_free(serverPubKey);
   
   if (SSH_OK == result)
   {
      keyHash.fHash = hash;
      keyHash.fLength = hashLength;
   }
   return result;
}


ASecureChannel* ASecureSession::OpenChannel()
{
   return new ASecureChannel(*this);
}


int ASecureSession::ReadFile(const std::string &srcPath, const std::string &dstPath,
                             ProgressFunc progress)
{
   ssh_session csession = fSession.getCSession();
   
   int result = SSH_ERROR;
   UniqueScpPtr scp(ssh_scp_new(csession, SSH_SCP_READ, srcPath.c_str()), EndScpSession);
   
   if (!scp)
   {
      ReportError(csession, "Error allocating scp session", SSH_ERROR);
   }
   else
   {
      result = ssh_scp_init(scp.get());
      ReportError(csession, "Error initializing scp session", result);
   }
   
   result = ssh_scp_pull_request(scp.get());
   if (SSH_SCP_REQUEST_NEWFILE != result)
   {
      ReportError(csession, "Error receiving information about file", result);
   }
   else
   {
      uint64_t fileSize = ssh_scp_request_get_size64(scp.get());
      std::ofstream out(dstPath + "/" + srcPath); //!!! Fix path construction
      if (out)
      {
         auto info { ProgressInfo(fileSize) };

         std::vector<uint8_t> buffer(32 * 1024);
         while (fileSize - info.completed > 0)
         {
            const auto readAmt = std::min((uint64_t)buffer.size(), fileSize);
            result = ssh_scp_read(scp.get(), buffer.data(), readAmt);
            if (SSH_ERROR == result)
            {
               ReportError(csession, "Failed to write read file from remote", result);
               break;
            }

            out.write((char*)buffer.data(), result);

            info.completed += result;
            progress(info);
         }
   
         // Check for eof.
         result = ssh_scp_pull_request(scp.get());
         if (result != SSH_SCP_REQUEST_EOF)
         {
            ReportError(csession, "Unexpected request during file read", result);
         }
         else
         {
            result = SSH_OK;
         }
      }
      else
      {
         ReportError(csession, "Failed to create local file to write to", SSH_ERROR);
      }
   }
   
   return result;
}


void ASecureSession::SetOptions(const std::string& user, const std::string& host,
                          int port, int verbosity)
{
   if (!user.empty())
   {
      fSession.setOption(SSH_OPTIONS_USER, user.c_str());
   }
   
   fSession.setOption(SSH_OPTIONS_HOST, host.c_str());
   fSession.setOption(SSH_OPTIONS_PORT, port);
   fSession.setOption(SSH_OPTIONS_LOG_VERBOSITY, verbosity);
   // TODO: Get the key type if the known hosts file has an entry for host.
   fSession.setOption(SSH_OPTIONS_HOSTKEYS, "ecdsa-sha2-nistp256,ssh-rsa,ssh-dsa");
   
   if (!fConfigFilename.empty())
   {
      fSession.optionsParseConfig(fConfigFilename.c_str());
   }
}


ssh::Session& ASecureSession::SshSession()
{
   return fSession;
}


int ASecureSession::VerifyKnownHost()
{
   ssh_session csession = fSession.getCSession();
   
   KeyHash keyHash;
   int result = this->HashServerPublicKey(keyHash);
   if (result == SSH_OK)
   {
      int state = fSession.isServerKnown();
      switch (state)
      {
         case SSH_KNOWN_HOSTS_OK:
         /* OK */
         break;
            
         case SSH_KNOWN_HOSTS_CHANGED:
         std::cerr << "Host key for server changed: it is now:\n";
         keyHash.Print();
         std::cerr << "For security reasons, connection will be stopped\n";
         result = SSH_ERROR;
         break;
            
         case SSH_KNOWN_HOSTS_OTHER:
         std::cerr << "The host key for this server was not found but another "
         "type of key exists.\n"
         "An attacker might change the default server key to "
         "confuse your client into thinking the key does not exist\n";
         keyHash.Print();
         result = SSH_ERROR;
         break;
            
         case SSH_KNOWN_HOSTS_NOT_FOUND:
         std::cerr << "Could not find known host file.\n"
         "If you accept the host key here, the file will be"
         "automatically created.\n";
         // FALL THROUGH to SSH_SERVER_NOT_KNOWN behavior
         [[fallthrough]];
            
         case SSH_KNOWN_HOSTS_UNKNOWN:
         result = this->AskIfServerTrusted(keyHash);
         break;
            
         case SSH_KNOWN_HOSTS_ERROR:
         std::cerr << "Error " << ssh_get_error(csession) << "\n";
         result = SSH_ERROR;
         break;
      }
   }
   
   return result;
}


int ASecureSession::WriteFile(const std::string& srcPath, size_t fileSize,
                        const std::string& dstPath, ProgressFunc progress)
{
   ssh_session csession = fSession.getCSession();
   
   int result = SSH_ERROR;
   UniqueScpPtr scp(ssh_scp_new(csession, SSH_SCP_WRITE | SSH_SCP_RECURSIVE, "."),
                    EndScpSession);
   
   if (!scp)
   {
      ReportError(csession, "Error allocating scp session", SSH_ERROR);
   }
   else
   {
      result = ssh_scp_init(scp.get());
      int code = ssh_get_error_code(csession);
      if (SSH_FATAL == code)
      {
         ReportError(csession, "Error initializing scp session", result);
      }
   }
   
   if (SSH_OK == result)
   {
      constexpr int perms = 0644;
      result = ssh_scp_push_file(scp.get(), dstPath.c_str(), fileSize, perms);
      ReportError(csession, "Error creating file on remote", result);
   }
   
   if (SSH_OK == result)
   {
      std::ifstream streamIn(srcPath, std::ios::in | std::ios::binary);
      std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(streamIn)),
                                  std::istreambuf_iterator<char>());

      auto info = ProgressInfo(buffer.size());
      progress(info);

      auto data = buffer.data();
      constexpr auto kWriteAmt = int64_t(16384UL);

      while (info.completed < fileSize)
      {
         auto writeCount = std::min(kWriteAmt, info.Remaining());
         assert(writeCount > 0 && writeCount <= info.total);
         result = ssh_scp_write(scp.get(), data, writeCount);
         ReportError(csession, "Failed to write file to remote", result);

         data += writeCount;
         info.completed += writeCount;
         progress(info);
      }
   }
   
   return result;
}


void ASecureSession::SetTrustServerFunc(TrustServerFunc trustFunc)
{
   fTrustServerFunc = trustFunc;
}


int ASecureSession::AskIfServerTrusted(const KeyHash& keyHash)
{
   // TODO: We'll probably have to do something fancy here like passing in
   // a blocking function to get user input. For now, console input.

   int result = SSH_OK;

   // Should we just store the fingerprint and manage trusting ourselves?
   // How would the user know if the fingerprint matches?

   std::unique_ptr<char, void(*)(char*)>
    hexa(ssh_get_hexa(keyHash.fHash, keyHash.fLength),
         ssh_string_free_char);

#if 1

   if (fTrustServerFunc && fTrustServerFunc(hexa.get()))
   {
      result = ssh_session_update_known_hosts(fSession.getCSession());
   }
   
#else // ask on command line


   std::cerr << "The server is unknown. Do you trust the host key?\n";
   std::cerr << "Public key hash: " <<  hexa << "\n";
   
   char buf[10] = { 0 };
   char* p = fgets(buf, sizeof(buf) - 1, stdin);
   if (p == NULL)
   {
      return -1;
   }
   
   int result = SSH_OK;
   if (strncasecmp(buf, "yes", 3) != 0)
   {
      result = SSH_ERROR;
   }
   else
   {
      result = ssh_session_update_known_hosts(fSession.getCSession());
   }
   
   if (SSH_OK != result)
   {
      std::cerr << "Error " << strerror(errno) << "\n";
   }

#endif

   return result;
}


//MARK: - ASecureChannel

ASecureChannel::ASecureChannel(ASecureSession& session)
: fChannel(session.SshSession())
{}


ASecureChannel::~ASecureChannel()
{
   try
   {
      if (fChannel.isOpen())
      {
         this->Close();
      }
   }
   catch (...)
   {
      // eat it
   }
}


void ASecureChannel::Close()
{
   fChannel.sendEof();
   fChannel.close();
}


void ASecureChannel::Execute(const std::string& command)
{
   if (!fChannel.isOpen())
   {
      this->Open();
   }
   fChannel.requestExec(command.c_str());
}


bool ASecureChannel::IsEndOfFile()
{
   return fChannel.isEof();
}


void ASecureChannel::Open()
{
   fChannel.openSession();
}


int ASecureChannel::Read(void* buffer, size_t count)
{
   assert(nullptr != buffer);
   if (nullptr == buffer)
   {
      return -1;
   }
   
   if (!fChannel.isOpen())
   {
      this->Open();
   }
   
   return fChannel.readNonblocking(buffer, count);
}


std::string ASecureChannel::ResultOfCommand(const std::string& command)
{
   std::string result;

   this->Execute(command);

   char buffer[4096];
   int readCount = 0;
   while (SSH_EOF != readCount)
   {
      memset(buffer, 0, readCount);
      readCount = this->Read(buffer, sizeof(buffer) - 1);

      if (readCount < 0 && readCount != SSH_EOF)
      {
         result += "\nRead failed with error " + std::to_string(readCount);
         break;
      }
      else if (0 < readCount)
      {
         result += buffer;
      }
   }
   return result;
}


bool ASecureChannel::ResultOfCommand(const std::string& command, MonitorFunc monitor)
{
   this->Execute(command);

   bool success = true;

   std::array<char, 4096> buffer { 0 };
   std::string longLine; // If a line does not occur in our buffer, accumulate it here.

   ptrdiff_t writePos = 0;
   int readCount = 0;
   while (SSH_EOF != readCount)
   {
      readCount = this->Read(buffer.data() + writePos, buffer.size() - writePos);

      if (readCount < 0)
      {
         success = (readCount == SSH_EOF);
         break;
      }

      const auto lines = StringUtils::SplitLines(buffer.data(), readCount + writePos);

      // When the entire buffer doesn't contain a line, store it for later.
      if (lines.empty())
      {
         longLine += std::string(buffer.data(), ::strnlen(buffer.data(), buffer.size()));
         writePos = 0;
         buffer[0] = '\0';
         continue;
      }

      size_t nextStart = 0;
      for (const auto& line : lines)
      {
         nextStart = line.data() - buffer.data() + line.size() + 1;

         if (!longLine.empty())
         {
            longLine += line;
            monitor(std::string_view(longLine.data(), longLine.size()));
            longLine.clear();

            continue;
         }

         monitor(line);
      }

      // Move an incomplete line to beginning of buffer.
      writePos = 0L;
      if (nextStart < readCount)
      {
         auto remainder = readCount - nextStart;
         strncpy(buffer.data(), buffer.data() + nextStart, remainder);
         writePos = remainder;
      }

      // Zero the rest of the buffer
      memset(buffer.data() + writePos, 0, buffer.size() - writePos);
   }

   if (success && !longLine.empty())
   {
      monitor(longLine);
   }
   return success;
}


// MARK: -  Static functions

namespace
{
   void EndScpSession(ssh_scp scp)
   {
      if (nullptr != scp)
      {
         ssh_scp_close(scp);
         ssh_scp_free(scp);
      }
   }
   
   void ReportError(ssh_session session, const char* const message, int result)
   {
      if (SSH_OK != result)
      {
         std::cerr << message << ": " << ssh_get_error(session) << "\n";
      }
   }
}
