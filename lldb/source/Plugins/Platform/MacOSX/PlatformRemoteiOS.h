//===-- PlatformRemoteiOS.h -------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_SOURCE_PLUGINS_PLATFORM_MACOSX_PLATFORMREMOTEIOS_H
#define LLDB_SOURCE_PLUGINS_PLATFORM_MACOSX_PLATFORMREMOTEIOS_H

#include "PlatformRemoteDarwinDevice.h"
#include "lldb/lldb-forward.h"
#include "llvm/ADT/StringRef.h"
#include "lldb/mix_device.h"

#include <vector>

namespace lldb_private {
class ArchSpec;

class PlatformRemoteiOS : public PlatformRemoteDarwinDevice {
public:
  PlatformRemoteiOS();

  static lldb::PlatformSP CreateInstance(bool force, const ArchSpec *arch);

  static void Initialize();

  static void Terminate();

  static llvm::StringRef GetPluginNameStatic() { return "remote-ios"; }

  static llvm::StringRef GetDescriptionStatic();

  llvm::StringRef GetDescription() override { return GetDescriptionStatic(); }

  llvm::StringRef GetPluginName() override { return GetPluginNameStatic(); }

  std::vector<ArchSpec>
  GetSupportedArchitectures(const ArchSpec &process_host_arch) override;

  Status ConnectRemote(lldb_private::Args &args) override;
  Status DisconnectRemote() override;
  bool IsConnected() const override;
  Status Install(const FileSpec &src, const FileSpec &dst) override;
  const char *GetHostname() override;
  lldb::ProcessSP DebugProcess(ProcessLaunchInfo &launch_info,
                               Debugger &debugger, Target &target,
                               Status &error) override;
  
  Status
  ResolveExecutable(const ModuleSpec &module_spec, lldb::ModuleSP &module_sp,
                    const FileSpecList *module_search_paths_ptr) override;

protected:
  bool CheckLocalSharedCache() const override;

  llvm::StringRef GetDeviceSupportDirectoryName() override;
  llvm::StringRef GetPlatformName() override;

private:
  mix_device_t* m_device;
  mix_lldb_debugger* m_lldb;
  std::string m_device_name;
};

} // namespace lldb_private

#endif // LLDB_SOURCE_PLUGINS_PLATFORM_MACOSX_PLATFORMREMOTEIOS_H
