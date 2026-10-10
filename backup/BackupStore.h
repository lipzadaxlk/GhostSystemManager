#pragma once

#include <QString>

class BackupStore {
public:
    static bool writeNetworkAddress(const QString& path, const QString& adapterGuid, bool existed, const QString& previous, QString* error);
    static bool readNetworkAddress(const QString& path, QString* adapterGuid, bool* existed, QString* previous, QString* error);
};
