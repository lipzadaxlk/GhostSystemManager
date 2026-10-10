#pragma once
#include <string>

struct ServiceResult { bool ok; unsigned long error; std::wstring message; };
ServiceResult installAndStartDriver(const std::wstring& serviceName, const std::wstring& imagePath);
ServiceResult stopAndRemoveDriver(const std::wstring& serviceName);
bool isAdministrator();

