#include "HardwareInventory.h"
#include <windows.h>

std::vector<HardwareRecord> readStorage() {
    std::vector<HardwareRecord> records;
    for(wchar_t letter=L'A';letter<=L'Z';++letter){ wchar_t root[4]={letter,L':',L'\\',0}; if(GetDriveTypeW(root)!=DRIVE_NO_ROOT_DIR) records.push_back({"Storage","volume",std::string(1,static_cast<char>(letter))+":","GetDriveTypeW",""}); }
    return records;
}

