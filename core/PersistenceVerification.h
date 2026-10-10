#pragma once

#include "AppTypes.h"

class PersistenceVerification {
public:
    static SystemSnapshot fromRecords(const QString& id, const QString& label, const std::vector<IdentityRecord>& records);
    static std::vector<SnapshotComparison> compare(const SystemSnapshot& before, const SystemSnapshot& after, bool afterReboot);
};
