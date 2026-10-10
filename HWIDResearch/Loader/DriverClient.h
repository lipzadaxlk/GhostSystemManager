#pragma once
#include <string>
#include "../Shared/Protocol.h"

struct ClientResult { bool ok; unsigned long error; std::wstring message; };
ClientResult queryDriver(const std::wstring& devicePath, HWID_DIAGNOSTICS* diagnostics, HWID_SYNTHETIC_IDENTITY* identity);
std::wstring findDeviceInterface();

