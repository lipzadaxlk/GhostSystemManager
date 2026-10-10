#pragma once

#include <string>

enum class HypervisorKind { None, VirtualBox, VMware, Qemu, HyperV };
struct HypervisorStatus { HypervisorKind kind = HypervisorKind::None; bool available = false; bool canConfigureVm = false; std::string executable; std::string detail; };
HypervisorStatus detectHypervisorBackend();
const char* hypervisorKindText(HypervisorKind kind);

