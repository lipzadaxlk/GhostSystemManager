#pragma once

#include <QString>
#include <QDateTime>
#include <vector>

enum class Mutability { ReadOnly, SupportedConfiguration, Unknown };

struct IdentityRecord {
    QString category;
    QString name;
    QString value;
    Mutability mutability = Mutability::ReadOnly;
    QString reason;
};

struct ChangePlan {
    QString adapterGuid;
    QString networkAddress;
    bool requiresAdministrator = true;
    bool valid = false;
    QString detail;
};

struct OperationResult {
    bool success = false;
    QString message;
    QDateTime timestamp = QDateTime::currentDateTime();
};

inline QString mutabilityText(Mutability value) {
    switch (value) {
    case Mutability::ReadOnly: return "Read-only";
    case Mutability::SupportedConfiguration: return "Supported configuration";
    case Mutability::Unknown: return "Unknown";
    }
    return "Unknown";
}
