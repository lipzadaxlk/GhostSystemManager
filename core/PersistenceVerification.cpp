#include "PersistenceVerification.h"

#include <QHash>
#include <QSet>

SystemSnapshot PersistenceVerification::fromRecords(const QString& id, const QString& label, const std::vector<IdentityRecord>& records) {
    SystemSnapshot snapshot; snapshot.id = id; snapshot.label = label;
    for (const auto& record : records) snapshot.fields.push_back({record.category + "/" + record.name, record.value, record.reason});
    return snapshot;
}

std::vector<SnapshotComparison> PersistenceVerification::compare(const SystemSnapshot& before, const SystemSnapshot& after, bool afterReboot) {
    QHash<QString, QString> left; QHash<QString, QString> right;
    for (const auto& field : before.fields) left.insert(field.key, field.value);
    for (const auto& field : after.fields) right.insert(field.key, field.value);
    QSet<QString> keys; for (auto iterator = left.keyBegin(); iterator != left.keyEnd(); ++iterator) keys.insert(*iterator); for (auto iterator = right.keyBegin(); iterator != right.keyEnd(); ++iterator) keys.insert(*iterator);
    std::vector<SnapshotComparison> result;
    for (const auto& key : keys) {
        const bool hasLeft = left.contains(key), hasRight = right.contains(key); const QString a = left.value(key), b = right.value(key);
        SnapshotComparison comparison{key, VerificationStatus::VerificationFailed, a, b, QString()};
        if (!hasLeft || !hasRight) comparison.status = VerificationStatus::Unsupported;
        else if (a == b) comparison.status = VerificationStatus::Unchanged;
        else if (afterReboot) comparison.status = VerificationStatus::Persistent;
        else comparison.status = VerificationStatus::Changed;
        comparison.detail = verificationStatusText(comparison.status); result.push_back(comparison);
    }
    return result;
}
