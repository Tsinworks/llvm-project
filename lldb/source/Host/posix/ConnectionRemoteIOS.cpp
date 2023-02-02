//===-- ConnectionFileDescriptorPosix.cpp ---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#if defined(__APPLE__)
// Enable this special support for Apple builds where we can have unlimited
// select bounds. We tried switching to poll() and kqueue and we were panicing
// the kernel, so we have to stick with select for now.
#define _DARWIN_UNLIMITED_SELECT
#endif

#include "lldb/Host/ConnectionRemoteIOS.h"
#include "lldb/Host/Config.h"
#include "lldb/Host/Socket.h"
#include "lldb/Host/SocketAddress.h"
#include "lldb/Utility/LLDBLog.h"
#include "lldb/Utility/SelectHelper.h"
#include "lldb/Utility/Timeout.h"
#include "lldb/mix_device.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/types.h>

#if LLDB_ENABLE_POSIX
#include <termios.h>
#include <unistd.h>
#endif

#include <memory>
#include <sstream>

#include "llvm/Support/Errno.h"
#include "llvm/Support/ErrorHandling.h"
#if defined(__APPLE__)
#include "llvm/ADT/SmallVector.h"
#endif
#include "lldb/Host/Host.h"
#include "lldb/Host/Socket.h"
#include "lldb/Host/common/TCPSocket.h"
#include "lldb/Host/common/UDPSocket.h"
#include "lldb/Utility/Log.h"
#include "lldb/Utility/StreamString.h"
#include "lldb/Utility/Timer.h"
#include "lldb/Utility/UriParser.h"

using namespace lldb;
using namespace lldb_private;

namespace lldb_private {
ConnectionRemoteIOS::ConnectionRemoteIOS() : m_dev(nullptr), m_conn(nullptr) {}
ConnectionRemoteIOS::~ConnectionRemoteIOS() {
  std::lock_guard<std::mutex> _(m_conn_lk);
  if (m_conn) {
    mix_lldb_connection_free(m_conn);
    m_conn = nullptr;
  }
  if (m_dev) {
    mix_release_device(m_dev);
    m_dev = nullptr;
  }
}
lldb::ConnectionStatus ConnectionRemoteIOS::Connect(llvm::StringRef url,
                                                    Status *error_ptr) {
  struct Result {
    Status *error_ptr;
    lldb::ConnectionStatus status;
  } res = {error_ptr};
  std::lock_guard<std::mutex> _(m_conn_lk);
  if (m_dev) {
    mix_release_device(m_dev);
    m_dev = nullptr;
  }
  std::optional<URI> parsed_url = URI::Parse(url);
  std::string new_url = (std::string)parsed_url->scheme + "://" + (std::string)parsed_url->hostname;
  mix_connect_device(
      new_url.c_str(), &m_dev,
      [](const char *error, void *usr_data) {
        Result *_res = (Result *)usr_data;
        if (error) {
          if (_res->error_ptr) {
            _res->error_ptr->Clear();
            _res->error_ptr->SetErrorString(error);
          }
          _res->status = lldb::eConnectionStatusError;
        } else {
          _res->status = lldb::eConnectionStatusSuccess;
        }
      },
      &res);

  if (m_conn) {
    mix_lldb_connection_free(m_conn);
    m_conn = nullptr;
  }

  if (res.status != lldb::eConnectionStatusSuccess) {
    return res.status;
  }

  m_conn = mix_device_create_lldb_connection(
      m_dev, "",
      [](mix_lldb_connection_status status, const char *error, void *usr_data) {
        Result *_res = (Result *)usr_data;
        _res->status = (lldb::ConnectionStatus)status;
        if (_res->error_ptr && error &&
            _res->status != lldb::eConnectionStatusSuccess) {
          _res->error_ptr->Clear();
          _res->error_ptr->SetErrorString(error);
        }
      },
      &res);
  return res.status;
}
lldb::ConnectionStatus ConnectionRemoteIOS::Disconnect(Status *error_ptr) {
  struct Result {
    Status *error_ptr;
    lldb::ConnectionStatus status;
  } res = {error_ptr, lldb::eConnectionStatusNoConnection};
  if (m_conn) {
    std::lock_guard<std::mutex> _(m_conn_lk);
    mix_lldb_connection_disconnect(
        m_conn,
        [](mix_lldb_connection_status status, const char *error,
           void *usr_data) {
          Result *_res = (Result *)usr_data;
          _res->status = (lldb::ConnectionStatus)status;
          if (_res->error_ptr && error &&
              _res->status != lldb::eConnectionStatusSuccess) {
            _res->error_ptr->Clear();
            _res->error_ptr->SetErrorString(error);
          }
        },
        &res);
  }
  return res.status;
}
bool ConnectionRemoteIOS::IsConnected() const {
  std::lock_guard<std::mutex> _(m_conn_lk);
  return m_conn ? mix_lldb_connection_is_connected(m_conn) : false;
  // return false;
}
size_t ConnectionRemoteIOS::Read(void *dst, size_t dst_len,
                                 const Timeout<std::micro> &timeout,
                                 lldb::ConnectionStatus &status,
                                 Status *error_ptr) {
  struct Result {
    Status *error_ptr;
    lldb::ConnectionStatus status;
  } res = {error_ptr};
  std::lock_guard<std::mutex> _(m_conn_lk);
  auto bytes = mix_lldb_connection_read(
      m_conn, dst, dst_len, timeout.value().count(),
      [](mix_lldb_connection_status status, const char *error, void *usr_data) {
        Result *_res = (Result *)usr_data;
        _res->status = (lldb::ConnectionStatus)status;
        if (_res->error_ptr && error) {
          _res->error_ptr->Clear();
          _res->error_ptr->SetErrorString(error);
        }
      },
      &res);
  status = res.status;
  return bytes;
}
size_t ConnectionRemoteIOS::Write(const void *dst, size_t dst_len,
                                  lldb::ConnectionStatus &status,
                                  Status *error_ptr) {
  struct Result {
    Status *error_ptr;
    lldb::ConnectionStatus status;
  } res = {error_ptr};
  std::lock_guard<std::mutex> _(m_conn_lk);
  auto bytes = mix_lldb_connection_write(
      m_conn, dst, dst_len,
      [](mix_lldb_connection_status status, const char *error, void *usr_data) {
        Result *_res = (Result *)usr_data;
        _res->status = (lldb::ConnectionStatus)status;
        if (_res->error_ptr && error) {
          _res->error_ptr->Clear();
          _res->error_ptr->SetErrorString(error);
        }
      },
      &res);
  status = res.status;
  return bytes;
}
std::string ConnectionRemoteIOS::GetURI() { return std::string(); }
bool ConnectionRemoteIOS::InterruptRead() {
  std::lock_guard<std::mutex> _(m_conn_lk);
  return m_conn ? mix_lldb_connection_interrupt_read(m_conn) : false;
  // return false;
}
lldb::IOObjectSP ConnectionRemoteIOS::GetReadObject() {
  return lldb::IOObjectSP();
}

} // namespace lldb_private
