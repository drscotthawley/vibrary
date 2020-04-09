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

#include <functional>
#include <string>

#include <libssh/libsshpp.hpp>


class ASecureChannel;
struct KeyHash;


/**
 * Interface to manage a secure session to a remote resource.
 *
 * All methods return 0 on success, anything else otherwise.
 */
class ASecureSession
{
public:
   
   static constexpr int kDefaultPort = 22;

   using TrustServerFunc = std::function<bool(const char* fingerprint)>;

   
   ASecureSession(const std::string& user, const std::string& host, int port = kDefaultPort,
                  const std::string& configFilename = "",
                  TrustServerFunc trustFunc = nullptr) noexcept(false);
   ~ASecureSession();

   /** Tracks bytes written or read */
   struct ProgressInfo
   {
      uint64_t completed = 0UL;
      const uint64_t total;
      constexpr int64_t Remaining() const { return total - completed; }
      ProgressInfo(uint64_t fileSize) : total(fileSize) {}
   };

   using ProgressFunc = std::function<void(const ProgressInfo&)>;

   int ReadFile(const std::string& srcPath, const std::string& dstPath,
                ProgressFunc);
   
   /**
    * Provides scp like behavior to copy a file from the local machine to
    * a remote one.
    */
   int WriteFile(const std::string& srcPath, size_t fileSize,
                 const std::string& dstPath, ProgressFunc);
   
   // Receiver owns the channel.
   ASecureChannel* OpenChannel();

   /** A function to validate whether to trust a server. */

   void SetTrustServerFunc(TrustServerFunc function);
   
private:
   
   friend class ASecureChannel;
   
   ssh::Session& SshSession();
   
   int Init(const std::string& user, const std::string& host, int port);
   void SetOptions(const std::string& user, const std::string& host, int port,
                   int verbosity);
   
   int VerifyKnownHost();
   int HashServerPublicKey(KeyHash& keyHash);
   int AskIfServerTrusted(const KeyHash& keyHash);
   
   int CleanUp();

   TrustServerFunc fTrustServerFunc;
   ssh::Session fSession;
   std::string fConfigFilename;
};


/** Manage input/output with a remote resource */
class ASecureChannel
{
public:

   using MonitorFunc = std::function<void(const std::string_view&)>;


   explicit ASecureChannel(ASecureSession& session);
   ~ASecureChannel();


   /**
    * Execute command and return result
    * \throws on error
    */
   std::string ResultOfCommand(const std::string& command);

   /**
    * Execute command and call MonitorFunc with each line read back from the server.
    * \return true if command results read successfully, false otherwise. May also throw.
    */
   bool ResultOfCommand(const std::string& command, MonitorFunc);
   
   /** Close the channel. Throws on error */
   void Close();
   
   // Low-level commands
   
   void Execute(const std::string& command);
   int Read(void* buffer, size_t maxLen);
   
   // ??? This doesn't seem to work. It returns false after Read has returned SSH_EOF
   bool IsEndOfFile();
   
   
private:
   // \throw on error
   void Open();

   ssh::Channel fChannel;
};

