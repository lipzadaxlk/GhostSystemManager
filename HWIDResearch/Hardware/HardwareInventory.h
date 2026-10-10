#pragma once
#include <string>
#include <vector>

struct HardwareRecord { std::string category; std::string name; std::string value; std::string source; std::string error; };
std::vector<HardwareRecord> scanReadOnlyHardware();

