#pragma once

#include <stdint.h>
#ifndef _KERNEL_MODE
#include <windows.h>
#include <winioctl.h>
#endif

#define HWID_PROTOCOL_VERSION 1u
#define HWID_DEVICE_INTERFACE_GUID \
    {0x4f86d8c1, 0x2eb0, 0x4bb8, {0x91, 0x52, 0x8a, 0x0c, 0x6b, 0x2d, 0x77, 0x31}}

#define IOCTL_HWID_GET_DIAGNOSTICS \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_READ_DATA)
#define IOCTL_HWID_GET_SYNTHETIC_IDENTITY \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_READ_DATA)

#define HWID_ID_MAX 64u

typedef struct _HWID_DIAGNOSTICS {
    uint32_t size;
    uint32_t version;
    uint32_t driverVersion;
    uint32_t flags;
} HWID_DIAGNOSTICS;

typedef struct _HWID_IDENTITY_REQUEST {
    uint32_t size;
    uint32_t version;
    uint64_t profileSeed;
} HWID_IDENTITY_REQUEST;

typedef struct _HWID_SYNTHETIC_IDENTITY {
    uint32_t size;
    uint32_t version;
    char systemUuid[HWID_ID_MAX];
    char macAddress[HWID_ID_MAX];
    char diskId[HWID_ID_MAX];
} HWID_SYNTHETIC_IDENTITY;

