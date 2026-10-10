#pragma once

#include "AppTypes.h"

class NetworkConfiguration {
public:
    static QString generateLocalAdminMac();
    static ChangePlan generatePlan(const QString& adapterGuid);
    static OperationResult apply(const ChangePlan& plan, const QString& backupPath);
    static OperationResult verify(const ChangePlan& plan);
    static OperationResult restore(const QString& adapterGuid, const QString& backupPath);
};
