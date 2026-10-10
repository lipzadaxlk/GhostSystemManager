#pragma once

#include "AppTypes.h"

class HardwareInventory {
public:
    std::vector<IdentityRecord> scan() const;
};
