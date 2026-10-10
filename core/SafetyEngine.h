#pragma once

#include "AppTypes.h"

class SafetyEngine {
public:
    static bool isAdministrator();
    static Capability computerNameCapability();
    static Capability networkAddressCapability(const QString& adapterGuid);
    static OperationResult validate(const Capability& capability);
};
