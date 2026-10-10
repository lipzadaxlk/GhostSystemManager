#pragma once

#include "AppTypes.h"

class WindowsConfiguration {
public:
    static QString currentComputerName();
    static OperationResult applyComputerName(const QString& proposedName);
    static OperationResult verifyComputerName(const QString& proposedName);
};
