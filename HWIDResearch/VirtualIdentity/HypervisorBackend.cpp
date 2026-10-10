#include "HypervisorBackend.h"
#ifdef _WIN32
#include <windows.h>
#endif

namespace {
std::string findTool(const wchar_t* name) {
#ifdef _WIN32
    wchar_t path[MAX_PATH]{}; const DWORD length=SearchPathW(nullptr,name,nullptr,MAX_PATH,path,nullptr); if(length>0&&length<MAX_PATH){std::wstring wide(path,length);return std::string(wide.begin(),wide.end());}
#else
    (void)name;
#endif
    return {};
}
}

HypervisorStatus detectHypervisorBackend() {
    if(const auto tool=findTool(L"VBoxManage.exe");!tool.empty()) return {HypervisorKind::VirtualBox,true,true,tool,"VirtualBox CLI detected; VM target still must be selected explicitly."};
    if(const auto tool=findTool(L"vmrun.exe");!tool.empty()) return {HypervisorKind::VMware,true,true,tool,"VMware CLI detected; VM target still must be selected explicitly."};
    if(const auto tool=findTool(L"qemu-system-x86_64.exe");!tool.empty()) return {HypervisorKind::Qemu,true,true,tool,"QEMU executable detected; configuration depends on the VM launch definition."};
    return {HypervisorKind::None,false,false,{},"No supported hypervisor CLI was detected. Hyper-V cmdlets also require the Hyper-V feature and elevation."};
}

const char* hypervisorKindText(HypervisorKind kind){switch(kind){case HypervisorKind::VirtualBox:return "VirtualBox";case HypervisorKind::VMware:return "VMware";case HypervisorKind::Qemu:return "QEMU";case HypervisorKind::HyperV:return "Hyper-V";default:return "None";}}

