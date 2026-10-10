#include "../Hardware/HardwareInventory.h"
#include <iostream>
int main(){ const auto records=scanReadOnlyHardware(); std::cout << "records=" << records.size() << "\n"; return records.empty()?1:0; }

