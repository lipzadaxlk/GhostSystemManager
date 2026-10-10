#include "../VirtualIdentity/IdentityProvider.h"
#include <iostream>

int main() {
    const auto first = getSyntheticIdentity(0x12345678ULL);
    const auto second = getSyntheticIdentity(0x12345678ULL);
    const auto other = getSyntheticIdentity(0x87654321ULL);
    const bool ok = isValidSyntheticIdentity(first) && first.systemUuid == second.systemUuid && first.macAddress == second.macAddress && first.diskId == second.diskId && first.systemUuid != other.systemUuid;
    std::cout << first.systemUuid << " | " << first.macAddress << " | " << first.diskId << "\n";
    return ok ? 0 : 1;
}

