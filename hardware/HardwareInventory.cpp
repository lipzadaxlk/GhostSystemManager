#include "HardwareInventory.h"

#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <winreg.h>
#include <setupapi.h>
#include <devguid.h>
#include <winioctl.h>
#include <ntddstor.h>
#include <wbemidl.h>
#include <tbs.h>
#include <string>
#include <vector>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "tbs.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

namespace {
QString wide(const wchar_t* value) { return QString::fromWCharArray(value ? value : L""); }
QString hex(const BYTE* bytes, ULONG length) {
    QString result;
    for (ULONG i = 0; i < length; ++i) result += QString("%1").arg(bytes[i], 2, 16, QLatin1Char('0')).toUpper();
    return result;
}
void add(std::vector<IdentityRecord>& records, const QString& category, const QString& name, const QString& value, Mutability mode, const QString& reason) {
    if (!value.isEmpty()) records.push_back({category, name, value, mode, reason});
}
bool hasRecord(const std::vector<IdentityRecord>& records, const QString& category, const QString& name) {
    for (const auto& record : records) if (record.category == category && record.name == name && !record.value.isEmpty()) return true;
    return false;
}
QString registryString(HKEY root, const wchar_t* path, const wchar_t* valueName) {
    wchar_t buffer[512]{}; DWORD bytes = sizeof(buffer); DWORD type = 0;
    if (RegGetValueW(root, path, valueName, RRF_RT_REG_SZ, &type, buffer, &bytes) == ERROR_SUCCESS) return wide(buffer);
    return {};
}
QString smbiosString(const BYTE* formatted, BYTE index, const BYTE* strings, const BYTE* end) {
    if (index == 0) return {};
    BYTE current = 1; const BYTE* p = strings;
    while (p < end && *p) { if (current == index) return QString::fromLatin1(reinterpret_cast<const char*>(p)); while (p < end && *p) ++p; if (p < end) ++p; ++current; }
    return {};
}
void scanSmbios(std::vector<IdentityRecord>& records) {
    const DWORD signature = ('R') | ('S' << 8) | ('M' << 16) | ('B' << 24);
    DWORD bytes = GetSystemFirmwareTable(signature, 0, nullptr, 0); if (bytes < 8) return;
    std::vector<BYTE> raw(bytes); if (GetSystemFirmwareTable(signature, 0, raw.data(), bytes) != bytes) return;
    const BYTE* p = raw.data() + 8; const BYTE* end = raw.data() + raw.size();
    while (p + 4 <= end) {
        BYTE type = p[0], length = p[1]; if (length < 4 || p + length > end) break;
        const BYTE* strings = p + length; const BYTE* next = strings;
        while (next + 1 < end && !(next[0] == 0 && next[1] == 0)) ++next;
        if (next + 1 >= end) break;
        if (type == 0 && length >= 6) { add(records, "BIOS", "Vendor", smbiosString(p, p[4], strings, end), Mutability::ReadOnly, "Firmware-provided SMBIOS BIOS value."); add(records, "BIOS", "Version", smbiosString(p, p[5], strings, end), Mutability::ReadOnly, "Firmware-provided SMBIOS BIOS value."); }
        if (type == 1 && length >= 24) { add(records, "System", "Manufacturer", smbiosString(p, p[4], strings, end), Mutability::ReadOnly, "Firmware-provided SMBIOS system value."); add(records, "System", "Product", smbiosString(p, p[5], strings, end), Mutability::ReadOnly, "Firmware-provided SMBIOS system value."); add(records, "System", "Serial", smbiosString(p, p[7], strings, end), Mutability::ReadOnly, "Firmware-provided SMBIOS system serial."); const QString uuid = hex(p + 8, 16); if (uuid.count('0') != uuid.size()) add(records, "SMBIOS", "System UUID", uuid, Mutability::ReadOnly, "Firmware-provided SMBIOS UUID; inspection only."); }
        if (type == 2 && length >= 8) { add(records, "Motherboard", "Manufacturer", smbiosString(p, p[4], strings, end), Mutability::ReadOnly, "Firmware-provided SMBIOS baseboard value."); add(records, "Motherboard", "Product", smbiosString(p, p[5], strings, end), Mutability::ReadOnly, "Firmware-provided SMBIOS baseboard value."); add(records, "Motherboard", "Serial", smbiosString(p, p[7], strings, end), Mutability::ReadOnly, "Firmware-provided SMBIOS baseboard serial."); }
        p = next + 2;
    }
}
QString variantString(const VARIANT& value) {
    switch (value.vt) {
    case VT_BSTR:
        return value.bstrVal ? QString::fromWCharArray(value.bstrVal) : QString();
    case VT_BOOL:
        return value.boolVal == VARIANT_TRUE ? QStringLiteral("True") : QStringLiteral("False");
    case VT_UI1: return QString::number(value.bVal);
    case VT_I2: return QString::number(value.iVal);
    case VT_UI2: return QString::number(value.uiVal);
    case VT_I4: return QString::number(value.lVal);
    case VT_UI4: return QString::number(value.ulVal);
    case VT_I8: return QString::number(value.llVal);
    case VT_UI8: return QString::number(value.ullVal);
    default: return {};
    }
}
QString wmiValueInNamespace(const wchar_t* namespacePath, const wchar_t* className, const wchar_t* propertyName) {
    HRESULT init = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(init) && init != RPC_E_CHANGED_MODE) return {};
    if (SUCCEEDED(init)) CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, nullptr);
    IWbemLocator* locator = nullptr; IWbemServices* services = nullptr; IEnumWbemClassObject* enumerator = nullptr; QString result;
    do {
        if (FAILED(CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&locator)))) break;
        BSTR namespaceName = SysAllocString(namespacePath);
        if (FAILED(locator->ConnectServer(namespaceName, nullptr, nullptr, nullptr, 0, nullptr, nullptr, &services))) { SysFreeString(namespaceName); break; }
        SysFreeString(namespaceName);
        BSTR language = SysAllocString(L"WQL"); QString queryText = QString("SELECT %1 FROM %2").arg(QString::fromWCharArray(propertyName), QString::fromWCharArray(className)); BSTR query = SysAllocString(reinterpret_cast<const wchar_t*>(queryText.utf16()));
        HRESULT queryStatus = services->ExecQuery(language, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, nullptr, &enumerator); SysFreeString(language); SysFreeString(query);
        if (FAILED(queryStatus)) break;
        IWbemClassObject* object = nullptr; ULONG returned = 0;
        if (enumerator->Next(WBEM_INFINITE, 1, &object, &returned) == S_OK && returned == 1) { VARIANT value; VariantInit(&value); if (SUCCEEDED(object->Get(propertyName, 0, &value, nullptr, nullptr))) result = variantString(value); VariantClear(&value); object->Release(); }
    } while (false);
    if (enumerator) enumerator->Release(); if (services) services->Release(); if (locator) locator->Release(); if (SUCCEEDED(init)) CoUninitialize(); return result.trimmed();
}
QString wmiValue(const wchar_t* className, const wchar_t* propertyName) { return wmiValueInNamespace(L"ROOT\\CIMV2", className, propertyName); }

QString validUuid(QString value) {
    value = value.trimmed();
    if (value.isEmpty()) return {};
    QString upper = value.toUpper();
    QString compact = upper;
    compact.remove('-').remove('{').remove('}');
    if (upper.contains(QStringLiteral("TO BE FILLED")) || compact.isEmpty() || compact.count('0') == compact.size()) return {};
    return value;
}
QString presentTpmDevice() {
    HDEVINFO devices = SetupDiGetClassDevsW(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (devices == INVALID_HANDLE_VALUE) return {};
    QString result;
    for (DWORD index = 0; ; ++index) {
        SP_DEVINFO_DATA data{}; data.cbSize = sizeof(data);
        if (!SetupDiEnumDeviceInfo(devices, index, &data)) break;
        wchar_t name[256]{}; DWORD type = 0, bytes = 0;
        if (SetupDiGetDeviceRegistryPropertyW(devices, &data, SPDRP_FRIENDLYNAME, &type, reinterpret_cast<PBYTE>(name), sizeof(name), &bytes) && QString::fromWCharArray(name).contains(QStringLiteral("Trusted Platform Module"), Qt::CaseInsensitive)) {
            result = QString::fromWCharArray(name).trimmed();
            break;
        }
    }
    SetupDiDestroyDeviceInfoList(devices);
    return result;
}
}

std::vector<IdentityRecord> HardwareInventory::scan() const {
    std::vector<IdentityRecord> records;
    scanSmbios(records);
    const QString wmiReason = "Read through the documented Windows WMI inventory provider.";
    const QString baseboardManufacturer = wmiValue(L"Win32_BaseBoard", L"Manufacturer"); if (!hasRecord(records, "Motherboard", "Manufacturer")) add(records, "Motherboard", "Manufacturer", baseboardManufacturer, Mutability::ReadOnly, wmiReason);
    const QString baseboardProduct = wmiValue(L"Win32_BaseBoard", L"Product"); if (!hasRecord(records, "Motherboard", "Product")) add(records, "Motherboard", "Product", baseboardProduct, Mutability::ReadOnly, wmiReason);
    const QString baseboardSerial = wmiValue(L"Win32_BaseBoard", L"SerialNumber"); if (!hasRecord(records, "Motherboard", "Serial")) add(records, "Motherboard", "Serial", baseboardSerial, Mutability::ReadOnly, wmiReason);
    const QString systemUuid = validUuid(wmiValue(L"Win32_ComputerSystemProduct", L"UUID")); if (!hasRecord(records, "SMBIOS", "System UUID")) add(records, "SMBIOS", "System UUID", systemUuid, Mutability::ReadOnly, wmiReason);
    const QString biosVendor = wmiValue(L"Win32_BIOS", L"Manufacturer"); if (!hasRecord(records, "BIOS", "Vendor")) add(records, "BIOS", "Vendor", biosVendor, Mutability::ReadOnly, wmiReason);
    const QString biosVersion = wmiValue(L"Win32_BIOS", L"SMBIOSBIOSVersion"); if (!hasRecord(records, "BIOS", "Version")) add(records, "BIOS", "Version", biosVersion, Mutability::ReadOnly, wmiReason);
    const QString biosSerial = wmiValue(L"Win32_BIOS", L"SerialNumber"); if (!hasRecord(records, "BIOS", "Serial")) add(records, "BIOS", "Serial", biosSerial, Mutability::ReadOnly, wmiReason);
    constexpr wchar_t biosPath[] = L"HARDWARE\\DESCRIPTION\\System\\BIOS";
    if (!hasRecord(records, "Motherboard", "Manufacturer")) add(records, "Motherboard", "Manufacturer", registryString(HKEY_LOCAL_MACHINE, biosPath, L"BaseBoardManufacturer"), Mutability::ReadOnly, "Read through the Windows hardware registry fallback.");
    if (!hasRecord(records, "Motherboard", "Product")) add(records, "Motherboard", "Product", registryString(HKEY_LOCAL_MACHINE, biosPath, L"BaseBoardProduct"), Mutability::ReadOnly, "Read through the Windows hardware registry fallback.");
    if (!hasRecord(records, "Motherboard", "Serial")) add(records, "Motherboard", "Serial", registryString(HKEY_LOCAL_MACHINE, biosPath, L"BaseBoardSerialNumber"), Mutability::ReadOnly, "Read through the Windows hardware registry fallback.");
    if (!hasRecord(records, "BIOS", "Vendor")) add(records, "BIOS", "Vendor", registryString(HKEY_LOCAL_MACHINE, biosPath, L"BIOSVendor"), Mutability::ReadOnly, "Read through the Windows hardware registry fallback.");
    if (!hasRecord(records, "BIOS", "Version")) add(records, "BIOS", "Version", registryString(HKEY_LOCAL_MACHINE, biosPath, L"BIOSVersion"), Mutability::ReadOnly, "Read through the Windows hardware registry fallback.");
    if (!hasRecord(records, "SMBIOS", "System UUID")) add(records, "SMBIOS", "System UUID", registryString(HKEY_LOCAL_MACHINE, biosPath, L"SystemUUID"), Mutability::ReadOnly, "Read through the Windows SMBIOS registry fallback.");
    wchar_t computer[256]{}; DWORD computerSize = 256;
    if (GetComputerNameExW(ComputerNamePhysicalDnsHostname, computer, &computerSize)) add(records, "Windows", "Computer name", wide(computer), Mutability::SupportedConfiguration, "Windows-supported computer-name configuration.");
    add(records, "Windows", "MachineGuid", registryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", L"MachineGuid"), Mutability::ReadOnly, "Installation identifier; the application does not modify it.");
    add(records, "Windows", "ProductId", registryString(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductId"), Mutability::ReadOnly, "Windows product identifier; inspection only.");

    SYSTEM_INFO system{}; GetNativeSystemInfo(&system);
    add(records, "CPU", "Architecture", QString::number(system.wProcessorArchitecture), Mutability::ReadOnly, "Hardware-reported architecture.");
    add(records, "CPU", "Identifier", registryString(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"Identifier"), Mutability::ReadOnly, "Hardware and OS-reported CPU descriptor.");
    add(records, "CPU", "ProcessorNameString", registryString(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"ProcessorNameString"), Mutability::ReadOnly, "Hardware and OS-reported CPU name.");

    for (DWORD index = 0;; ++index) {
        DISPLAY_DEVICEW display{}; display.cb = sizeof(display);
        if (!EnumDisplayDevicesW(nullptr, index, &display, 0)) break;
        add(records, "GPU", wide(display.DeviceString), wide(display.DeviceID), Mutability::ReadOnly, "Display driver device identifier; inspection only.");
    }

    ULONG adapterBytes = 0; GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, nullptr, &adapterBytes);
    std::vector<BYTE> adapterBuffer(adapterBytes);
    auto* adapter = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(adapterBuffer.data());
    if (GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, nullptr, adapter, &adapterBytes) == NO_ERROR) {
        for (auto* current = adapter; current; current = current->Next) {
            QString mac = current->PhysicalAddressLength ? hex(current->PhysicalAddress, current->PhysicalAddressLength) : QString();
            QString adapterName = wide(current->FriendlyName);
            add(records, "Network", adapterName + " MAC", mac, Mutability::SupportedConfiguration, "Some physical adapters accept a documented driver NetworkAddress setting; this is adapter and driver dependent.");
            add(records, "Network", adapterName + " GUID", QString::fromLatin1(current->AdapterName), Mutability::ReadOnly, "Adapter instance identity; inspection only.");
        }
    }

    for (DWORD index = 0; index < 32; ++index) {
        QString path = QString("\\\\.\\PhysicalDrive%1").arg(index);
        HANDLE disk = CreateFileW(reinterpret_cast<LPCWSTR>(path.utf16()), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        if (disk == INVALID_HANDLE_VALUE) continue;
        STORAGE_PROPERTY_QUERY query{}; query.PropertyId = StorageDeviceProperty; query.QueryType = PropertyStandardQuery;
        BYTE output[2048]{}; DWORD returned = 0;
        if (DeviceIoControl(disk, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), output, sizeof(output), &returned, nullptr)) {
            auto* descriptor = reinterpret_cast<STORAGE_DEVICE_DESCRIPTOR*>(output);
            auto field = [output, descriptor](DWORD offset) { return offset ? QString::fromLatin1(reinterpret_cast<const char*>(output + offset)).trimmed() : QString(); };
            add(records, "Disk", QString("PhysicalDrive%1 model").arg(index), field(descriptor->ProductIdOffset), Mutability::ReadOnly, "Storage hardware descriptor; inspection only.");
            add(records, "Disk", QString("PhysicalDrive%1 serial").arg(index), field(descriptor->SerialNumberOffset), Mutability::ReadOnly, "Storage hardware serial; inspection only.");
        }
        CloseHandle(disk);
    }

    wchar_t volumePath[MAX_PATH]{}; HANDLE volume = FindFirstVolumeW(volumePath, MAX_PATH);
    if (volume != INVALID_HANDLE_VALUE) {
        do {
            DWORD serial = 0; wchar_t label[MAX_PATH]{};
            if (GetVolumeInformationW(volumePath, label, MAX_PATH, &serial, nullptr, nullptr, nullptr, 0)) add(records, "Volume", wide(label), QString("%1").arg(serial, 8, 16, QLatin1Char('0')).toUpper(), Mutability::ReadOnly, "Filesystem serial; inspection only.");
        } while (FindNextVolumeW(volume, volumePath, MAX_PATH));
        FindVolumeClose(volume);
    }

    const wchar_t tpmNamespace[] = L"ROOT\\CIMV2\\Security\\MicrosoftTpm";
    const QString tpmEnabled = wmiValueInNamespace(tpmNamespace, L"Win32_Tpm", L"IsEnabled_InitialValue");
    const QString tpmActivated = wmiValueInNamespace(tpmNamespace, L"Win32_Tpm", L"IsActivated_InitialValue");
    const QString tpmOwned = wmiValueInNamespace(tpmNamespace, L"Win32_Tpm", L"IsOwned_InitialValue");
    const QString tpmSpec = wmiValueInNamespace(tpmNamespace, L"Win32_Tpm", L"SpecVersion");
    TBS_CONTEXT_PARAMS2 params{}; params.version = TBS_CONTEXT_VERSION_TWO; params.includeTpm12 = 1; params.includeTpm20 = 1; TBS_HCONTEXT context{};
    const bool tbsAvailable = Tbsi_Context_Create(reinterpret_cast<PCTBS_CONTEXT_PARAMS>(&params), &context) == TBS_SUCCESS;
    if (tbsAvailable) Tbsip_Context_Close(context);
    const QString tpmDevice = presentTpmDevice();
    QString tpmState;
    if (!tpmEnabled.isEmpty() || !tpmActivated.isEmpty() || !tpmOwned.isEmpty() || !tpmSpec.isEmpty()) {
        tpmState = QString("Enabled: %1 | Activated: %2 | Owned: %3 | Spec: %4")
            .arg(tpmEnabled.isEmpty() ? QStringLiteral("Unknown") : tpmEnabled,
                 tpmActivated.isEmpty() ? QStringLiteral("Unknown") : tpmActivated,
                 tpmOwned.isEmpty() ? QStringLiteral("Unknown") : tpmOwned,
                 tpmSpec.isEmpty() ? QStringLiteral("Unknown") : tpmSpec);
    } else if (tbsAvailable) {
        tpmState = QStringLiteral("Present (TBS available; Win32_Tpm state unavailable)");
    } else if (!tpmDevice.isEmpty()) {
        tpmState = QStringLiteral("Present (device manager: %1; WMI/TBS state unavailable)").arg(tpmDevice);
    } else {
        tpmState = QStringLiteral("Not exposed by WMI/TBS");
    }
    add(records, "TPM", "TPM state", tpmState, Mutability::ReadOnly, "Read through documented Windows TPM interfaces; no TPM identity is changed.");
    return records;
}
