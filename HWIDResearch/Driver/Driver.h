#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <initguid.h>
#include <wdm.h>
#include <ntstrsafe.h>
#include <wdmsec.h>
#include "../Shared/Protocol.h"

DEFINE_GUID(GUID_DEVINTERFACE_HWID_RESEARCH, 0x4f86d8c1, 0x2eb0, 0x4bb8, 0x91, 0x52, 0x8a, 0x0c, 0x6b, 0x2d, 0x77, 0x31);

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD HwidEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP HwidEvtDriverContextCleanup;
EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL HwidEvtIoDeviceControl;

