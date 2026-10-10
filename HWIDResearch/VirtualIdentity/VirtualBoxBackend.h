#pragma once

#include "IdentityProfile.h"
#include <string>
#include <vector>

struct VirtualBoxVm { std::string name; std::string uuid; std::string state; };
struct VirtualBoxStatus { bool installed=false; std::string executable; std::string detail; };
struct VirtualBoxApplyResult { bool success=false; bool applied=false; bool rebootRequired=false; bool guestVerified=false; std::string detail; std::vector<std::string> configured; std::vector<std::string> unsupported; };

VirtualBoxStatus detectVirtualBox();
std::vector<VirtualBoxVm> listVirtualBoxVms(const VirtualBoxStatus& status, std::string* error=nullptr);
VirtualBoxApplyResult applyVirtualBoxIdentity(const VirtualBoxStatus& status, const VirtualBoxVm& vm, const IdentityProfile& profile, bool restartVm);

