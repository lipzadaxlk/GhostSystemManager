#include "HardwareInventory.h"
#include <windows.h>
#include <string>

std::vector<HardwareRecord> readSmbios() {
    std::vector<HardwareRecord> records; UINT size=GetSystemFirmwareTable('RSMB',0,nullptr,0);
    if(!size){ records.push_back({"SMBIOS","table","","GetSystemFirmwareTable","unavailable"}); return records; }
    std::string detail="SMBIOS table available ("+std::to_string(size)+" bytes)";
    records.push_back({"SMBIOS","raw table",detail,"GetSystemFirmwareTable",""}); return records;
}

