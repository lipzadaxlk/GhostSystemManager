#include "Driver.h"

VOID HwidEvtIoDeviceControl(WDFQUEUE queue, WDFREQUEST request, size_t outputLength, size_t inputLength, ULONG code) {
    UNREFERENCED_PARAMETER(queue);
    if (code != IOCTL_HWID_GET_DIAGNOSTICS && code != IOCTL_HWID_GET_SYNTHETIC_IDENTITY) {
        WdfRequestComplete(request, STATUS_INVALID_DEVICE_REQUEST);
        return;
    }
    size_t required = code == IOCTL_HWID_GET_DIAGNOSTICS ? sizeof(HWID_DIAGNOSTICS) : sizeof(HWID_SYNTHETIC_IDENTITY);
    if (inputLength < (code == IOCTL_HWID_GET_SYNTHETIC_IDENTITY ? sizeof(HWID_IDENTITY_REQUEST) : 0) || outputLength < required) {
        WdfRequestComplete(request, STATUS_BUFFER_TOO_SMALL);
        return;
    }
    PVOID buffer = NULL;
    size_t length = 0;
    if (code == IOCTL_HWID_GET_SYNTHETIC_IDENTITY) {
        PVOID input = NULL;
        size_t inputSize = 0;
        NTSTATUS inputStatus = WdfRequestRetrieveInputBuffer(request, sizeof(HWID_IDENTITY_REQUEST), &input, &inputSize);
        if (!NT_SUCCESS(inputStatus)) { WdfRequestComplete(request, inputStatus); return; }
        HWID_IDENTITY_REQUEST* identityRequest = (HWID_IDENTITY_REQUEST*)input;
        if (identityRequest->size != sizeof(HWID_IDENTITY_REQUEST) || identityRequest->version != HWID_PROTOCOL_VERSION) {
            WdfRequestComplete(request, STATUS_REVISION_MISMATCH);
            return;
        }
    }
    NTSTATUS status = WdfRequestRetrieveOutputBuffer(request, required, &buffer, &length);
    if (!NT_SUCCESS(status)) { WdfRequestComplete(request, status); return; }
    RtlZeroMemory(buffer, required);
    if (code == IOCTL_HWID_GET_DIAGNOSTICS) {
        HWID_DIAGNOSTICS* response = (HWID_DIAGNOSTICS*)buffer;
        response->size = sizeof(*response); response->version = HWID_PROTOCOL_VERSION; response->driverVersion = 1; response->flags = 1;
    } else {
        HWID_SYNTHETIC_IDENTITY* response = (HWID_SYNTHETIC_IDENTITY*)buffer;
        response->size = sizeof(*response); response->version = HWID_PROTOCOL_VERSION;
        RtlStringCchCopyA(response->systemUuid, HWID_ID_MAX, "SYNTHETIC-ONLY-DRIVER");
        RtlStringCchCopyA(response->macAddress, HWID_ID_MAX, "02:00:00:00:00:01");
        RtlStringCchCopyA(response->diskId, HWID_ID_MAX, "SYNTH-DRIVER-DIAGNOSTIC");
    }
    WdfRequestCompleteWithInformation(request, STATUS_SUCCESS, required);
}

