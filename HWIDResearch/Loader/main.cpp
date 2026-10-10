#include "ServiceManager.h"
#include "DriverClient.h"
#include <iostream>
#include <windows.h>

int wmain(int argc, wchar_t** argv) {
    if (argc > 2 && std::wstring(argv[1]) == L"--install") {
        const auto result=installAndStartDriver(L"HWIDResearch", argv[2]); std::wcerr << result.message << L"\n"; return result.ok?0:static_cast<int>(result.error);
    }
    if (argc > 1 && std::wstring(argv[1]) == L"--remove") {
        const auto result=stopAndRemoveDriver(L"HWIDResearch"); std::wcerr << result.message << L"\n"; return result.ok?0:static_cast<int>(result.error);
    }
    if (argc > 1 && std::wstring(argv[1]) == L"--status") {
        const auto path=findDeviceInterface(); if(path.empty()){ std::wcerr << L"Driver interface is not present.\n"; return 2; }
        HWID_DIAGNOSTICS diagnostics{}; HWID_SYNTHETIC_IDENTITY identity{}; const auto result=queryDriver(path,&diagnostics,&identity);
        std::wcout << result.message << L"\n"; if(!result.ok) return 3;
        std::wcout << L"Version: " << diagnostics.driverVersion << L"\nSynthetic UUID: " << identity.systemUuid << L"\nSynthetic MAC: " << identity.macAddress << L"\nSynthetic disk: " << identity.diskId << L"\n"; return 0;
    }
    std::wcout << L"HWIDResearch loader (status-only by default).\n";
    std::wcout << L"Usage: HWIDResearchLoader.exe --status | --install <absolute-sys-path> | --remove\n";
    std::wcout << L"Installation is intentionally not automatic; use the documented WDK test VM procedure.\n";
    return 0;
}

