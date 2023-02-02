#pragma once

#include "lldb/Utility/Connection.h"
#include "lldb/lldb-types.h"
#include "lldb/mix_device.h"
#include <mutex>

namespace lldb_private {
class Status;
class ConnectionRemoteIOS : public lldb_private::Connection {
public:
	ConnectionRemoteIOS();
	~ConnectionRemoteIOS() override;

	lldb::ConnectionStatus Connect(llvm::StringRef url,
		Status* error_ptr) override;

	lldb::ConnectionStatus Disconnect(Status* error_ptr) override;

	bool IsConnected() const override;

	size_t Read(void* dst, size_t dst_len,
		const Timeout<std::micro>& timeout,
		lldb::ConnectionStatus& status, Status* error_ptr) override;

	size_t Write(const void* dst, size_t dst_len,
		lldb::ConnectionStatus& status, Status* error_ptr) override;

	std::string GetURI() override;

	bool InterruptRead() override;

	lldb::IOObjectSP GetReadObject() override;
private:
	mix_device_t* m_dev;
	mix_lldb_debugger* m_conn;
	mutable std::mutex m_conn_lk;
};
}