#pragma once

#include "AppTypes.h"

class SnapshotStore {
public:
    static bool write(const QString& path, const SystemSnapshot& snapshot, QString* error);
    static bool read(const QString& path, SystemSnapshot* snapshot, QString* error);
};
