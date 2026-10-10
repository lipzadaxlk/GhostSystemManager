#pragma once

#include "AppTypes.h"

class OperationLogger {
public:
    explicit OperationLogger(QString path);
    void write(const QString& action, const OperationResult& result) const;
    QString readAll() const;
private:
    QString path_;
};
