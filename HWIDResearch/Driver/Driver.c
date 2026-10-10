#include "Driver.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath) {
    WDF_DRIVER_CONFIG config;
    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_DRIVER_CONFIG_INIT(&config, HwidEvtDeviceAdd);
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = HwidEvtDriverContextCleanup;
    return WdfDriverCreate(driverObject, registryPath, &attributes, &config, WDF_NO_HANDLE);
}

VOID HwidEvtDriverContextCleanup(WDFOBJECT object) {
    UNREFERENCED_PARAMETER(object);
}

