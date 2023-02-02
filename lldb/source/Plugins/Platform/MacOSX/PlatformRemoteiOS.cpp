//===-- PlatformRemoteiOS.cpp ---------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "PlatformRemoteiOS.h"

#include "Plugins/Platform/gdb-server/PlatformRemoteGDBServer.h"
#include "lldb/Breakpoint/BreakpointLocation.h"
#include "lldb/Core/Module.h"
#include "lldb/Core/ModuleList.h"
#include "lldb/Core/ModuleSpec.h"
#include "lldb/Core/PluginManager.h"
#include "lldb/Host/ConnectionRemoteIOS.h"
#include "lldb/Host/Host.h"
#include "lldb/Target/Process.h"
#include "lldb/Target/Target.h"
#include "lldb/Utility/ArchSpec.h"
#include "lldb/Utility/FileSpec.h"
#include "lldb/Utility/LLDBLog.h"
#include "lldb/Utility/Log.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/StreamString.h"
#include "lldb/Utility/UriParser.h"

using namespace lldb;
using namespace lldb_private;

namespace lldb_private {
class Status;
class ConnectionRemoteLLDB : public Connection {
public:
  ConnectionRemoteLLDB(mix_lldb_debugger *m_lldb);
  ~ConnectionRemoteLLDB() override;

  lldb::ConnectionStatus Connect(llvm::StringRef url,
                                 Status *error_ptr) override;

  lldb::ConnectionStatus Disconnect(Status *error_ptr) override;

  bool IsConnected() const override;

  size_t Read(void *dst, size_t dst_len, const Timeout<std::micro> &timeout,
              lldb::ConnectionStatus &status, Status *error_ptr) override;

  size_t Write(const void *dst, size_t dst_len, lldb::ConnectionStatus &status,
               Status *error_ptr) override;

  std::string GetURI() override;

  bool InterruptRead() override;

  lldb::IOObjectSP GetReadObject() override;

  // std::string GetRemotePath(const llvm::StringRef app,
  // lldb::ConnectionStatus& status, Status* error_ptr);

private:
  mix_lldb_debugger *m_lldb;
};
} // namespace lldb_private

LLDB_PLUGIN_DEFINE(PlatformRemoteiOS)

// Static Variables
static uint32_t g_initialize_count = 0;

// Static Functions
void PlatformRemoteiOS::Initialize() {
  PlatformDarwin::Initialize();

  if (g_initialize_count++ == 0) {
    PluginManager::RegisterPlugin(PlatformRemoteiOS::GetPluginNameStatic(),
                                  PlatformRemoteiOS::GetDescriptionStatic(),
                                  PlatformRemoteiOS::CreateInstance);
  }
}

void PlatformRemoteiOS::Terminate() {
  if (g_initialize_count > 0) {
    if (--g_initialize_count == 0) {
      PluginManager::UnregisterPlugin(PlatformRemoteiOS::CreateInstance);
    }
  }

  PlatformDarwin::Terminate();
}

PlatformSP PlatformRemoteiOS::CreateInstance(bool force, const ArchSpec *arch) {
  Log *log = GetLog(LLDBLog::Platform);
  if (log) {
    const char *arch_name;
    if (arch && arch->GetArchitectureName())
      arch_name = arch->GetArchitectureName();
    else
      arch_name = "<null>";

    const char *triple_cstr =
        arch ? arch->GetTriple().getTriple().c_str() : "<null>";

    LLDB_LOGF(log, "PlatformRemoteiOS::%s(force=%s, arch={%s,%s})",
              __FUNCTION__, force ? "true" : "false", arch_name, triple_cstr);
  }

  bool create = force;
  if (!create && arch && arch->IsValid()) {
    switch (arch->GetMachine()) {
    case llvm::Triple::arm:
    case llvm::Triple::aarch64:
    case llvm::Triple::thumb: {
      const llvm::Triple &triple = arch->GetTriple();
      llvm::Triple::VendorType vendor = triple.getVendor();
      switch (vendor) {
      case llvm::Triple::Apple:
        create = true;
        break;

#if defined(__APPLE__)
      // Only accept "unknown" for the vendor if the host is Apple and
      // "unknown" wasn't specified (it was just returned because it was NOT
      // specified)
      case llvm::Triple::UnknownVendor:
        create = !arch->TripleVendorWasSpecified();
        break;

#endif
      default:
        break;
      }
      if (create) {
        switch (triple.getOS()) {
        case llvm::Triple::Darwin: // Deprecated, but still support Darwin for
                                   // historical reasons
        case llvm::Triple::IOS:    // This is the right triple value for iOS
                                   // debugging
          break;

        default:
          create = false;
          break;
        }
      }
    } break;
    default:
      break;
    }
  }

  if (create) {
    if (log)
      LLDB_LOGF(log, "PlatformRemoteiOS::%s() creating platform", __FUNCTION__);

    return lldb::PlatformSP(new PlatformRemoteiOS());
  }

  if (log)
    LLDB_LOGF(log, "PlatformRemoteiOS::%s() aborting creation of platform",
              __FUNCTION__);

  return lldb::PlatformSP();
}

llvm::StringRef PlatformRemoteiOS::GetDescriptionStatic() {
  return "Remote iOS platform plug-in.";
}

/// Default Constructor
PlatformRemoteiOS::PlatformRemoteiOS()
    : PlatformRemoteDarwinDevice(), m_device(nullptr), m_lldb(nullptr) {}

std::vector<ArchSpec> PlatformRemoteiOS::GetSupportedArchitectures(
    const ArchSpec &process_host_arch) {
  std::vector<ArchSpec> result;
  ARMGetSupportedArchitectures(result, llvm::Triple::IOS);
  return result;
}

Status PlatformRemoteiOS::ConnectRemote(lldb_private::Args &args) {
  const char *url = args.GetArgumentAtIndex(0);
  if (!url)
    return Status("URL is null.");

  if (m_device) {
    mix_release_device(m_device);
    m_device = nullptr;
  }

  mix_connect_device(url, &m_device, nullptr, nullptr);

  Status error;

  if (m_device) {
    struct Result {
      Status *error_ptr;
      lldb::ConnectionStatus status;
    } res = {&error};

    if (m_lldb) {
      mix_lldb_connection_free(m_lldb);
      m_lldb = nullptr;
    }

    m_lldb = mix_device_create_lldb_connection(
        m_device, "",
        [](mix_lldb_connection_status status, const char *error,
           void *usr_data) {
          Result *_res = (Result *)usr_data;
          _res->status = (lldb::ConnectionStatus)status;
          if (_res->error_ptr && error) {
            _res->error_ptr->Clear();
            _res->error_ptr->SetErrorString(error);
          }
        },
        &res);
  }

  if (!m_remote_platform_sp)
    m_remote_platform_sp =
        platform_gdb_server::PlatformRemoteGDBServer::CreateInstance(
            /*force=*/true, nullptr /*, new ConnectionRemoteLLDB(m_lldb)*/);

  if (m_remote_platform_sp && error.Success())
    error = m_remote_platform_sp->ConnectRemote(args);
  else
    error.SetErrorString("failed to create a 'remote-gdb-server' platform");

  if (error.Fail())
    m_remote_platform_sp.reset();

  if (error.Success() && m_remote_platform_sp) {
    if (m_option_group_platform_rsync.get() &&
        m_option_group_platform_ssh.get() &&
        m_option_group_platform_caching.get()) {
      if (m_option_group_platform_rsync->m_rsync) {
        SetSupportsRSync(true);
        SetRSyncOpts(m_option_group_platform_rsync->m_rsync_opts.c_str());
        SetRSyncPrefix(m_option_group_platform_rsync->m_rsync_prefix.c_str());
        SetIgnoresRemoteHostname(
            m_option_group_platform_rsync->m_ignores_remote_hostname);
      }
      if (m_option_group_platform_ssh->m_ssh) {
        SetSupportsSSH(true);
        SetSSHOpts(m_option_group_platform_ssh->m_ssh_opts.c_str());
      }
      SetLocalCacheDirectory(
          m_option_group_platform_caching->m_cache_dir.c_str());
    }
  }
  return error;
}

Status PlatformRemoteiOS::DisconnectRemote() {
  if (m_lldb) {
    mix_lldb_connection_free(m_lldb);
    m_lldb = nullptr;
  }
  return PlatformRemoteDarwinDevice::DisconnectRemote();
}

bool PlatformRemoteiOS::IsConnected() const {
  return m_lldb ? mix_lldb_connection_is_connected(m_lldb) : false;
}

Status PlatformRemoteiOS::Install(const FileSpec &src, const FileSpec &dst) {
  Status error;
  return error;
}

const char *PlatformRemoteiOS::GetHostname() {
  if (m_device) {
    auto name = mix_device_get_name(m_device);
    m_device_name = name;
    mix_free_string(name);
  }
  return m_device_name.c_str();
}

lldb::ProcessSP PlatformRemoteiOS::DebugProcess(ProcessLaunchInfo &launch_info,
                                                Debugger &debugger,
                                                Target &target, Status &error) {
  lldb::ProcessSP process_sp;
  if (m_remote_platform_sp)
    process_sp = m_remote_platform_sp->DebugProcess(launch_info, debugger,
                                                    target, error);
  else
    error.SetErrorString("the platform is not currently connected");

  return process_sp;
  //  return PlatformRemoteDarwinDevice::DebugProcess(launch_info, debugger,
  //  target, error);
}

Status PlatformRemoteiOS::ResolveExecutable(
    const ModuleSpec &module_spec, lldb::ModuleSP &module_sp,
    const FileSpecList *module_search_paths_ptr) {
  // PlatformRemoteDarwinDevice::ResolveRemotePath();
  // PlatformRemoteDarwinDevice::ResolveRemoteExecutable()
  auto fileSpec = module_spec.GetFileSpec();
  auto path = fileSpec.GetPath();
  struct Result {
    lldb::ModuleSP &module_sp;
    std::string path;
    PlatformRemoteiOS *thiz;
    Status error;
  } res = {module_sp, path, this};
  if (m_device) {
    mix_device_resolve_executable(
        m_device, path.c_str(),
        [](const char *app, const char *exe, const char *remote_path,
           void *usr) {
          Result *res = (Result *)usr;
          std::string full = res->path + "/" + exe;
          FileSpec fs(full);
          ModuleSpec module_spec(fs);
          if (FileSystem::Instance().Exists(module_spec.GetFileSpec())) {
            if (module_spec.GetArchitecture().IsValid() ||
                module_spec.GetUUID().IsValid()) {
              res->error = ModuleList::GetSharedModule(
                  module_spec, res->module_sp, nullptr, nullptr, nullptr);

              if (res->module_sp && res->module_sp->GetObjectFile())
                return;
              res->module_sp.reset();
            }
            // No valid architecture was specified or the exact ARM slice wasn't
            // found so ask the platform for the architectures that we should be
            // using (in the correct order) and see if we can find a match that
            // way
            StreamString arch_names;
            llvm::ListSeparator LS;
            ArchSpec process_host_arch;
            for (const ArchSpec &arch :
                 res->thiz->GetSupportedArchitectures(process_host_arch)) {
              module_spec.GetArchitecture() = arch;
              res->error = ModuleList::GetSharedModule(
                  module_spec, res->module_sp, nullptr, nullptr, nullptr);
              // Did we find an executable using one of the
              if (res->error.Success()) {
                if (res->module_sp && res->module_sp->GetObjectFile())
                  break;
                else
                  res->error.SetErrorToGenericError();
              }

              arch_names << LS << arch.GetArchitectureName();
            }
            std::string remote = std::string(remote_path) + "/" + exe;
            res->module_sp->SetPlatformFileSpec(FileSpec(remote));
            res->module_sp->SetRemoteInstallFileSpec(FileSpec(remote_path));

            if (res->error.Fail() || !res->module_sp) {
              if (FileSystem::Instance().Readable(module_spec.GetFileSpec())) {
                res->error.SetErrorStringWithFormatv(
                    "'{0}' doesn't contain any '{1}' platform architectures: "
                    "{2}",
                    module_spec.GetFileSpec(), res->thiz->GetPluginName(),
                    arch_names.GetData());
              } else {
                res->error.SetErrorStringWithFormat(
                    "'%s' is not readable",
                    module_spec.GetFileSpec().GetPath().c_str());
              }
            } // end check
          }
        },
        [](const char *error, void *usr) {
          Result *res = (Result *)usr;
          res->error.SetErrorString(error);
        },
        &res);
  }
  return res.error;
}

bool PlatformRemoteiOS::CheckLocalSharedCache() const {
  // You can run iPhone and iPad apps on Mac with Apple Silicon. At the
  // platform level there's no way to distinguish them from remote iOS
  // applications. Make sure we still read from our own shared cache.
  return true;
}

llvm::StringRef PlatformRemoteiOS::GetDeviceSupportDirectoryName() {
  return "iOS DeviceSupport";
}

llvm::StringRef PlatformRemoteiOS::GetPlatformName() {
  return "iPhoneOS.platform";
}

namespace lldb_private {
ConnectionRemoteLLDB::ConnectionRemoteLLDB(mix_lldb_debugger *lldb)
    : m_lldb(lldb) {}
ConnectionRemoteLLDB::~ConnectionRemoteLLDB() {}
lldb::ConnectionStatus ConnectionRemoteLLDB::Connect(llvm::StringRef url,
                                                     Status *error_ptr) {
  return lldb::ConnectionStatus();
}
lldb::ConnectionStatus ConnectionRemoteLLDB::Disconnect(Status *error_ptr) {
  return lldb::ConnectionStatus();
}
bool ConnectionRemoteLLDB::IsConnected() const {
  return mix_lldb_connection_is_connected(m_lldb);
}
size_t ConnectionRemoteLLDB::Read(void *dst, size_t dst_len,
                                  const Timeout<std::micro> &timeout,
                                  lldb::ConnectionStatus &status,
                                  Status *error_ptr) {
  struct Result {
    Status *error_ptr;
    lldb::ConnectionStatus status;
  } res = {error_ptr};
  auto bytes = mix_lldb_connection_read(
      m_lldb, dst, dst_len, timeout.value().count(),
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
size_t ConnectionRemoteLLDB::Write(const void *dst, size_t dst_len,
                                   lldb::ConnectionStatus &status,
                                   Status *error_ptr) {
  struct Result {
    Status *error_ptr;
    lldb::ConnectionStatus status;
  } res = {error_ptr};
  auto bytes = mix_lldb_connection_write(
      m_lldb, dst, dst_len,
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
std::string ConnectionRemoteLLDB::GetURI() { return std::string(); }
bool ConnectionRemoteLLDB::InterruptRead() {
  return mix_lldb_connection_interrupt_read(m_lldb);
}
lldb::IOObjectSP ConnectionRemoteLLDB::GetReadObject() {
  return lldb::IOObjectSP();
}
} // namespace lldb_private