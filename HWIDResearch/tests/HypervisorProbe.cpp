#include "../VirtualIdentity/HypervisorBackend.h"
#include <iostream>
int main(){const auto status=detectHypervisorBackend();std::cout<<"backend="<<hypervisorKindText(status.kind)<<" available="<<(status.available?"true":"false")<<" configurable="<<(status.canConfigureVm?"true":"false")<<" detail="<<status.detail<<"\n";return 0;}

