#include "IdentityProvider.h"

SyntheticIdentity getSyntheticIdentity(std::uint64_t profileSeed) {
    return generateSyntheticIdentity(profileSeed);
}

