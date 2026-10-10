#include "HardwareInventory.h"
#include <windows.h>

std::vector<HardwareRecord> readGpu() {
    return {{"GPU","enumeration","Use SetupAPI display-class query in the host application","documented read-only boundary",""}};
}

