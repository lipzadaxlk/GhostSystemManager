#include "BackupStore.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

bool BackupStore::writeNetworkAddress(const QString& path, const QString& adapterGuid, bool existed, const QString& previous, QString* error) {
    QJsonObject object{{"adapterGuid", adapterGuid}, {"valueExisted", existed}, {"networkAddress", previous}};
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) { if (error) *error = file.errorString(); return false; }
    file.write(QJsonDocument(object).toJson(QJsonDocument::Indented)); return true;
}
bool BackupStore::readNetworkAddress(const QString& path, QString* adapterGuid, bool* existed, QString* previous, QString* error) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) { if (error) *error = file.errorString(); return false; }
    QJsonParseError parse{}; auto document = QJsonDocument::fromJson(file.readAll(), &parse);
    if (parse.error != QJsonParseError::NoError || !document.isObject()) { if (error) *error = "Invalid backup JSON."; return false; }
    auto object = document.object(); *adapterGuid = object.value("adapterGuid").toString(); *existed = object.value("valueExisted").toBool(); *previous = object.value("networkAddress").toString();
    return !adapterGuid->isEmpty();
}
