#include "ServiceManager.h"
#include "DriverClient.h"
#include "../VirtualIdentity/VirtualBoxBackend.h"
#include <iostream>
#include <windows.h>
#include <chrono>
#include <algorithm>
#include <cstdint>
#include <cwchar>

int wmain(int argc, wchar_t** argv) {
    if (argc > 1 && std::wstring(argv[1]) == L"--vms") {
        const auto status=detectVirtualBox(); if(!status.installed){std::cerr<<status.detail<<"\nInstall VirtualBox from https://www.virtualbox.org/wiki/Downloads and ensure VBoxManage.exe is on PATH.\n";return 2;}
        std::string error; const auto vms=listVirtualBoxVms(status,&error); if(!error.empty()){std::cerr<<error<<"\n";return 3;} for(const auto& vm:vms)std::cout<<vm.name<<" | "<<vm.uuid<<"\n";return 0;
    }
    if (argc > 2 && std::wstring(argv[1]) == L"--apply") {
        const auto status=detectVirtualBox(); if(!status.installed){std::cerr<<status.detail<<"\nInstall VirtualBox from https://www.virtualbox.org/wiki/Downloads and ensure VBoxManage.exe is on PATH.\n";return 2;}
        const std::string selected=std::string(argv[2],argv[2]+wcslen(argv[2])); std::string error; const auto vms=listVirtualBoxVms(status,&error); auto found=std::find_if(vms.begin(),vms.end(),[&](const auto& vm){return vm.name==selected||vm.uuid==selected;}); if(found==vms.end()){std::cerr<<"VM not found. Use --vms to list existing VMs.\n";return 3;}
        const auto seed=static_cast<std::uint64_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count()); const auto profile=generateIdentityProfile(seed); const auto result=applyVirtualBoxIdentity(status,*found,profile,true); std::cout<<(result.applied?"APPLIED":"FAILED")<<"\n"<<result.detail<<"\n"; for(const auto& item:result.configured)std::cout<<"CONFIGURED: "<<item<<"\n"; for(const auto& item:result.unsupported)std::cout<<"UNSUPPORTED: "<<item<<"\n"; return result.success?0:(result.applied?4:5);
    }
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

