#include "NetworkConfiguration.h"
#include "BackupStore.h"

#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <iphlpapi.h>
#include <QDir>
#include <QFileInfo>
#include <QRandomGenerator>
#include <vector>

#pragma comment(lib, "iphlpapi.lib")

namespace {
constexpr wchar_t kClassPath[] = L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}";
QString normalized(const QString& value) { QString copy = value; copy.remove('{'); copy.remove('}'); copy.remove('-'); return copy.toUpper(); }
bool elevated() {
    HANDLE token{}; TOKEN_ELEVATION elevation{}; DWORD size = 0;
    bool ok = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token) && GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size) && elevation.TokenIsElevated;
    if (token) CloseHandle(token); return ok;
}
bool findAdapterKey(const QString& guid, HKEY* key, QString* error) {
    HKEY base{}; if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, kClassPath, 0, KEY_READ | KEY_WRITE, &base) != ERROR_SUCCESS) { *error = "Unable to open the network adapter registry class key."; return false; }
    for (DWORD index = 0;; ++index) {
        wchar_t child[256]{}; DWORD size = 256;
        LONG status = RegEnumKeyExW(base, index, child, &size, nullptr, nullptr, nullptr, nullptr);
        if (status == ERROR_NO_MORE_ITEMS) break;
        if (status != ERROR_SUCCESS) continue;
        HKEY candidate{}; if (RegOpenKeyExW(base, child, 0, KEY_READ | KEY_WRITE, &candidate) != ERROR_SUCCESS) continue;
        wchar_t instance[256]{}; DWORD bytes = sizeof(instance), type = 0;
        if (RegQueryValueExW(candidate, L"NetCfgInstanceId", nullptr, &type, reinterpret_cast<BYTE*>(instance), &bytes) == ERROR_SUCCESS && type == REG_SZ && normalized(QString::fromWCharArray(instance)) == normalized(guid)) { RegCloseKey(base); *key = candidate; return true; }
        RegCloseKey(candidate);
    }
    RegCloseKey(base); *error = "The selected adapter does not expose a configurable NetworkAddress driver key."; return false;
}
}

QString NetworkConfiguration::generateLocalAdminMac() {
    QByteArray bytes(6, Qt::Uninitialized);
    for (int i = 0; i < bytes.size(); ++i) bytes[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    bytes[0] = static_cast<char>((static_cast<unsigned char>(bytes[0]) & 0xFE) | 0x02);
    QString result; for (unsigned char byte : bytes) result += QString("%1").arg(byte, 2, 16, QLatin1Char('0')).toUpper();
    return result;
}
ChangePlan NetworkConfiguration::generatePlan(const QString& adapterGuid) {
    ChangePlan plan; plan.adapterGuid = adapterGuid; plan.networkAddress = generateLocalAdminMac(); plan.valid = !adapterGuid.isEmpty();
    plan.detail = plan.valid ? "Prepared a locally administered unicast MAC. Apply writes only the selected adapter driver's NetworkAddress setting." : "Select a network adapter GUID first.";
    return plan;
}
OperationResult NetworkConfiguration::apply(const ChangePlan& plan, const QString& backupPath) {
    if (!plan.valid || plan.networkAddress.size() != 12) return {false, "No valid network configuration plan exists."};
    if (!elevated()) return {false, "Administrator elevation is required. Restart the application as administrator, then confirm Apply Changes."};
    HKEY key{}; QString error; if (!findAdapterKey(plan.adapterGuid, &key, &error)) return {false, error};
    wchar_t previous[256]{}; DWORD bytes = sizeof(previous), type = 0; bool existed = RegQueryValueExW(key, L"NetworkAddress", nullptr, &type, reinterpret_cast<BYTE*>(previous), &bytes) == ERROR_SUCCESS && type == REG_SZ;
    QDir().mkpath(QFileInfo(backupPath).absolutePath());
    if (!BackupStore::writeNetworkAddress(backupPath, plan.adapterGuid, existed, existed ? QString::fromWCharArray(previous) : QString(), &error)) { RegCloseKey(key); return {false, "Backup failed: " + error}; }
    const auto utf16 = plan.networkAddress.toStdWString();
    LONG status = RegSetValueExW(key, L"NetworkAddress", 0, REG_SZ, reinterpret_cast<const BYTE*>(utf16.c_str()), static_cast<DWORD>((utf16.size() + 1) * sizeof(wchar_t)));
    RegCloseKey(key);
    if (status != ERROR_SUCCESS) return {false, QString("Registry write failed with Windows error %1.").arg(status)};
    return {true, "NetworkAddress was written. Windows or the driver may require adapter disable/enable or restart before it is active; the application will not restart networking automatically."};
}
OperationResult NetworkConfiguration::verify(const ChangePlan& plan) {
    if (!plan.valid) return {false, "No valid plan exists to verify."};
    ULONG bytes = 0; GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, nullptr, &bytes); std::vector<BYTE> buffer(bytes);
    auto* adapter = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
    if (GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, adapter, &bytes) != NO_ERROR) return {false, "GetAdaptersAddresses failed."};
    for (auto* current = adapter; current; current = current->Next) if (normalized(QString::fromLatin1(current->AdapterName)) == normalized(plan.adapterGuid)) {
        QString observed; for (ULONG i = 0; i < current->PhysicalAddressLength; ++i) observed += QString("%1").arg(current->PhysicalAddress[i], 2, 16, QLatin1Char('0')).toUpper();
        return {observed == plan.networkAddress, observed == plan.networkAddress ? "Verified: the active adapter MAC matches the requested value." : "Not active yet: observed " + observed + ". The driver may not support NetworkAddress or may require a manual adapter restart."};
    }
    return {false, "Selected adapter was not found during independent verification."};
}
OperationResult NetworkConfiguration::restore(const QString& adapterGuid, const QString& backupPath) {
    if (!elevated()) return {false, "Administrator elevation is required to restore a driver configuration."};
    QString backedGuid, previous, error; bool existed = false;
    if (!BackupStore::readNetworkAddress(backupPath, &backedGuid, &existed, &previous, &error)) return {false, "Backup read failed: " + error};
    if (normalized(backedGuid) != normalized(adapterGuid)) return {false, "Backup belongs to a different adapter."};
    HKEY key{}; if (!findAdapterKey(adapterGuid, &key, &error)) return {false, error};
    LONG status = existed ? RegSetValueExW(key, L"NetworkAddress", 0, REG_SZ, reinterpret_cast<const BYTE*>(previous.utf16()), static_cast<DWORD>((previous.size() + 1) * sizeof(wchar_t))) : RegDeleteValueW(key, L"NetworkAddress");
    RegCloseKey(key); if (status != ERROR_SUCCESS && !(status == ERROR_FILE_NOT_FOUND && !existed)) return {false, QString("Restore failed with Windows error %1.").arg(status)};
    return {true, "Original NetworkAddress configuration was restored. A manual adapter restart may be required for the driver to reload it."};
}
