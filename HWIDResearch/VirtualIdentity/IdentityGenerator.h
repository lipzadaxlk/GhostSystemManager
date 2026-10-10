#pragma once

#include <cstdint>
#include <string>

struct SyntheticIdentity {
    std::string systemUuid;
    std::string macAddress;
    std::string diskId;
};

SyntheticIdentity generateSyntheticIdentity(std::uint64_t seed);
bool isValidSyntheticIdentity(const SyntheticIdentity& identity);

