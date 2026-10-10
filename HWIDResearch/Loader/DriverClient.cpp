#include "DriverClient.h"
#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <vector>

DEFINE_GUID(GUID_DEVINTERFACE_HWID_RESEARCH, 0x4f86d8c1, 0x2eb0, 0x4bb8, 0x91, 0x52, 0x8a, 0x0c, 0x6b, 0x2d, 0x77, 0x31);
namespace { std::wstring errorText(DWORD error) { wchar_t buffer[512]{}; FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,nullptr,error,0,buffer,512,nullptr); return buffer; } }

std::wstring findDeviceInterface() {
    HDEVINFO set = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_HWID_RESEARCH, nullptr, nullptr, DIGCF_PRESENT|DIGCF_DEVICEINTERFACE); if (set == INVALID_HANDLE_VALUE) return {};
    SP_DEVICE_INTERFACE_DATA data{}; data.cbSize=sizeof(data); std::wstring path;
    if (SetupDiEnumDeviceInterfaces(set,nullptr,&GUID_DEVINTERFACE_HWID_RESEARCH,0,&data)) { DWORD required=0; SetupDiGetDeviceInterfaceDetailW(set,&data,nullptr,0,&required,nullptr); if(required){ std::vector<BYTE> buffer(required); auto* detail=reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W*>(buffer.data()); detail->cbSize=sizeof(*detail); if(SetupDiGetDeviceInterfaceDetailW(set,&data,detail,required,nullptr,nullptr)) path=detail->DevicePath; } }
    SetupDiDestroyDeviceInfoList(set); return path;
}

ClientResult queryDriver(const std::wstring& devicePath, HWID_DIAGNOSTICS* diagnostics, HWID_SYNTHETIC_IDENTITY* identity) {
    HANDLE device=CreateFileW(devicePath.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr); if(device==INVALID_HANDLE_VALUE){auto e=GetLastError();return{false,e,errorText(e)};}
    DWORD returned=0; bool ok=DeviceIoControl(device,IOCTL_HWID_GET_DIAGNOSTICS,nullptr,0,diagnostics,sizeof(*diagnostics),&returned,nullptr) != FALSE;
    HWID_IDENTITY_REQUEST request{}; request.size=sizeof(request); request.version=HWID_PROTOCOL_VERSION; request.profileSeed=0x4857494452455345ULL;
    if(ok) ok=DeviceIoControl(device,IOCTL_HWID_GET_SYNTHETIC_IDENTITY,&request,sizeof(request),identity,sizeof(*identity),&returned,nullptr) != FALSE;
    const auto e=ok?ERROR_SUCCESS:GetLastError(); CloseHandle(device); return {ok,e,ok?L"Driver diagnostics and synthetic identity received.":errorText(e)};
}

