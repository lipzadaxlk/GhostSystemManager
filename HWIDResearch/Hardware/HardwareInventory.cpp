#include "HardwareInventory.h"

std::vector<HardwareRecord> readSmbios();
std::vector<HardwareRecord> readStorage();
std::vector<HardwareRecord> readNetwork();
std::vector<HardwareRecord> readGpu();

std::vector<HardwareRecord> scanReadOnlyHardware() {
    auto result=readSmbios(); auto append=[&](auto&& items){ result.insert(result.end(),items.begin(),items.end()); };
    append(readStorage()); append(readNetwork()); append(readGpu()); return result;
}

