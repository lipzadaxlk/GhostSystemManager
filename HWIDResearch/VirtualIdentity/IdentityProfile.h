#pragma once

#include <cstdint>
#include <string>

struct IdentityProfile {
    std::string profileId;
    std::string systemUuid;
    std::string biosVendor;
    std::string biosVersion;
    std::string biosSerial;
    std::string baseboardManufacturer;
    std::string baseboardProduct;
    std::string baseboardSerial;
    std::string storageId;
    std::string networkGuid;
    std::string macAddress;
    std::string gpuId;
    std::string volumeId;
    std::string installationId;
};

IdentityProfile generateIdentityProfile(std::uint64_t seed);
bool validateIdentityProfile(const IdentityProfile& profile, std::string* error = nullptr);
bool saveIdentityProfile(const std::string& path, const IdentityProfile& profile, std::string* error = nullptr);
bool loadIdentityProfile(const std::string& path, IdentityProfile* profile, std::string* error = nullptr);

