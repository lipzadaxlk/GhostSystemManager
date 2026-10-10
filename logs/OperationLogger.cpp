#include "OperationLogger.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

OperationLogger::OperationLogger(QString path) : path_(std::move(path)) {}
void OperationLogger::write(const QString& action, const OperationResult& result) const {
    QDir().mkpath(QFileInfo(path_).absolutePath()); QFile file(path_);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) { QTextStream stream(&file); stream << result.timestamp.toString(Qt::ISODate) << " | " << action << " | " << (result.success ? "SUCCESS" : "FAILURE") << " | " << result.message << '\n'; }
}
QString OperationLogger::readAll() const { QFile file(path_); return file.open(QIODevice::ReadOnly | QIODevice::Text) ? QString::fromUtf8(file.readAll()) : QString("No operation log exists yet."); }
