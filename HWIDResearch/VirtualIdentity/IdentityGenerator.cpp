#include "IdentityGenerator.h"

#include <array>
#include <iomanip>
#include <sstream>

namespace {
std::uint64_t mix(std::uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
}
}

SyntheticIdentity generateSyntheticIdentity(std::uint64_t seed) {
    const auto a = mix(seed ^ 0x534d42494f530001ULL);
    const auto b = mix(seed ^ 0x4d41435f54455354ULL);
    const auto c = mix(seed ^ 0x4449534b5f544553ULL);
    std::ostringstream uuid;
    uuid << std::uppercase << std::hex << std::setfill('0')
         << std::setw(8) << static_cast<std::uint32_t>(a >> 32) << '-'
         << std::setw(4) << static_cast<std::uint16_t>(a >> 16) << '-'
         << std::setw(4) << static_cast<std::uint16_t>(a | 0x4000) << '-'
         << std::setw(4) << static_cast<std::uint16_t>(b | 0x8000) << '-'
         << std::setw(12) << (c & 0xffffffffffffULL);
    std::ostringstream mac;
    mac << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
        << ((b >> 40 & 0xfc) | 0x02);
    for (int shift = 32; shift >= 0; shift -= 8) mac << ':' << std::setw(2) << ((b >> shift) & 0xff);
    std::ostringstream disk;
    disk << "SYNTH-" << std::uppercase << std::hex << std::setfill('0') << std::setw(16) << c;
    return {uuid.str(), mac.str(), disk.str()};
}

bool isValidSyntheticIdentity(const SyntheticIdentity& identity) {
    return identity.systemUuid.size() == 36 && identity.macAddress.size() == 17 && identity.diskId.rfind("SYNTH-", 0) == 0;
}

