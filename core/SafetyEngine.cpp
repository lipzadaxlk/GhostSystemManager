#include "SafetyEngine.h"

#include <windows.h>

bool SafetyEngine::isAdministrator() {
    HANDLE token{}; TOKEN_ELEVATION elevation{}; DWORD size = 0;
    const bool opened = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token) != FALSE;
    const bool elevated = opened && GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size) != FALSE && elevation.TokenIsElevated != 0;
    if (token) CloseHandle(token);
    return elevated;
}

Capability SafetyEngine::computerNameCapability() {
    return {"Computer name", true, true, true, true, "Uses SetComputerNameExW; Windows may require a later restart."};
}

Capability SafetyEngine::networkAddressCapability(const QString& adapterGuid) {
    if (adapterGuid.trimmed().isEmpty()) return {"NetworkAddress", false, true, false, true, "No adapter GUID was selected."};
    return {"NetworkAddress", true, true, false, true, "Driver-dependent registry setting; active MAC must be independently verified."};
}

OperationResult SafetyEngine::validate(const Capability& capability) {
    if (!capability.supported) return {false, capability.name + " is not supported: " + capability.reason};
    if (capability.requiresAdministrator && !isAdministrator()) return {false, "Administrator elevation is required for " + capability.name + "."};
    return {true, "Safety checks passed for " + capability.name + "."};
}
