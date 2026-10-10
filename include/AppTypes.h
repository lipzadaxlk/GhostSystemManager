#pragma once

#include <QString>
#include <QDateTime>
#include <vector>
#include <QJsonObject>

enum class Mutability { ReadOnly, SupportedConfiguration, Unknown };

struct IdentityRecord {
    QString category;
    QString name;
    QString value;
    Mutability mutability = Mutability::ReadOnly;
    QString reason;
    QString source;
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

enum class VerificationStatus { Unchanged, Changed, Persistent, Reverted, Unsupported, VerificationFailed };

struct SnapshotField {
    QString key;
    QString value;
    QString source;
};

struct SystemSnapshot {
    QString id;
    QString label;
    QDateTime timestamp = QDateTime::currentDateTimeUtc();
    std::vector<SnapshotField> fields;
};

struct SnapshotComparison {
    QString key;
    VerificationStatus status = VerificationStatus::VerificationFailed;
    QString before;
    QString after;
    QString detail;
};

struct Capability {
    QString name;
    bool supported = false;
    bool requiresAdministrator = false;
    bool requiresReboot = false;
    bool reversible = false;
    QString reason;
};

inline QString verificationStatusText(VerificationStatus value) {
    switch (value) {
    case VerificationStatus::Unchanged: return "UNCHANGED";
    case VerificationStatus::Changed: return "CHANGED";
    case VerificationStatus::Persistent: return "PERSISTENT";
    case VerificationStatus::Reverted: return "REVERTED";
    case VerificationStatus::Unsupported: return "UNSUPPORTED";
    case VerificationStatus::VerificationFailed: return "VERIFICATION FAILED";
    }
    return "VERIFICATION FAILED";
}

inline QString mutabilityText(Mutability value) {
    switch (value) {
    case Mutability::ReadOnly: return "Read-only";
    case Mutability::SupportedConfiguration: return "Supported configuration";
    case Mutability::Unknown: return "Unknown";
    }
    return "Unknown";
}
