#define _WIN32_WINNT 0x0601
#include "HardwareInventory.h"
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <vector>

std::vector<HardwareRecord> readNetwork() {
    std::vector<HardwareRecord> records; ULONG size=0; GetAdaptersAddresses(AF_UNSPEC,0,nullptr,nullptr,&size); if(!size) return records;
    std::vector<BYTE> buffer(size); auto* adapters=reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data()); if(GetAdaptersAddresses(AF_UNSPEC,0,nullptr,adapters,&size)!=NO_ERROR) return records;
    for(auto* item=adapters;item;item=item->Next){ char mac[32]{}; sprintf_s(mac,"%02X:%02X:%02X:%02X:%02X:%02X",item->PhysicalAddress[0],item->PhysicalAddress[1],item->PhysicalAddress[2],item->PhysicalAddress[3],item->PhysicalAddress[4],item->PhysicalAddress[5]); records.push_back({"Network",item->AdapterName,mac,"GetAdaptersAddresses",""}); }
    return records;
}

