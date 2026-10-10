#include "SnapshotStore.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>

bool SnapshotStore::write(const QString& path, const SystemSnapshot& snapshot, QString* error) {
    QJsonArray fields;
    for (const auto& field : snapshot.fields) fields.append(QJsonObject{{"key", field.key}, {"value", field.value}, {"source", field.source}});
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) { if (error) *error = file.errorString(); return false; }
    const QJsonObject root{{"id", snapshot.id}, {"label", snapshot.label}, {"timestamp", snapshot.timestamp.toString(Qt::ISODateWithMs)}, {"fields", fields}};
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    if (!file.commit()) { if (error) *error = file.errorString(); return false; }
    return true;
}

bool SnapshotStore::read(const QString& path, SystemSnapshot* snapshot, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) { if (error) *error = file.errorString(); return false; }
    QJsonParseError parse{}; const auto document = QJsonDocument::fromJson(file.readAll(), &parse);
    if (parse.error != QJsonParseError::NoError || !document.isObject()) { if (error) *error = "Invalid snapshot JSON."; return false; }
    const auto object = document.object(); snapshot->id = object.value("id").toString(); snapshot->label = object.value("label").toString(); snapshot->timestamp = QDateTime::fromString(object.value("timestamp").toString(), Qt::ISODateWithMs); snapshot->fields.clear();
    for (const auto& item : object.value("fields").toArray()) { const auto field = item.toObject(); snapshot->fields.push_back({field.value("key").toString(), field.value("value").toString(), field.value("source").toString()}); }
    if (snapshot->id.isEmpty() || snapshot->fields.empty()) { if (error) *error = "Snapshot is missing an id or fields."; return false; }
    return true;
}
