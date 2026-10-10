#include "ServiceManager.h"
#include <windows.h>
#include <sddl.h>
#include <string>

namespace { std::wstring message(unsigned long error) { wchar_t buffer[512]{}; FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error, 0, buffer, 512, nullptr); return buffer; } }

bool isAdministrator() {
    BOOL member = FALSE; PSID administrators = nullptr;
    SID_IDENTIFIER_AUTHORITY authority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&authority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0,0,0,0,0,0, &administrators)) {
        CheckTokenMembership(nullptr, administrators, &member); FreeSid(administrators);
    }
    return member == TRUE;
}

ServiceResult installAndStartDriver(const std::wstring& serviceName, const std::wstring& imagePath) {
    if (!isAdministrator()) return {false, ERROR_ACCESS_DENIED, L"Administrator privileges are required."};
    SC_HANDLE manager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
    if (!manager) return {false, GetLastError(), message(GetLastError())};
    SC_HANDLE service = CreateServiceW(manager, serviceName.c_str(), serviceName.c_str(), SERVICE_START | SERVICE_STOP | DELETE,
        SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL, imagePath.c_str(), nullptr, nullptr, nullptr, nullptr, nullptr);
    if (!service && GetLastError() == ERROR_SERVICE_EXISTS) service = OpenServiceW(manager, serviceName.c_str(), SERVICE_START | SERVICE_STOP | DELETE);
    if (!service) { const auto e=GetLastError(); CloseServiceHandle(manager); return {false,e,message(e)}; }
    const bool started = StartServiceW(service, 0, nullptr) || GetLastError() == ERROR_SERVICE_ALREADY_RUNNING;
    const auto e = started ? ERROR_SUCCESS : GetLastError();
    CloseServiceHandle(service); CloseServiceHandle(manager);
    return {started, e, started ? L"Driver service started." : message(e)};
}

ServiceResult stopAndRemoveDriver(const std::wstring& serviceName) {
    if (!isAdministrator()) return {false, ERROR_ACCESS_DENIED, L"Administrator privileges are required."};
    SC_HANDLE manager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT); if (!manager) return {false,GetLastError(),message(GetLastError())};
    SC_HANDLE service = OpenServiceW(manager, serviceName.c_str(), SERVICE_STOP | DELETE | SERVICE_QUERY_STATUS); if (!service) { const auto e=GetLastError(); CloseServiceHandle(manager); return {false,e,message(e)}; }
    SERVICE_STATUS status{}; ControlService(service, SERVICE_CONTROL_STOP, &status); const bool removed = DeleteService(service) != FALSE; const auto e=removed?ERROR_SUCCESS:GetLastError(); CloseServiceHandle(service); CloseServiceHandle(manager);
    return {removed,e,removed?L"Driver service stopped and removed.":message(e)};
}

