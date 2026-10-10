#pragma once
#include "IdentityProfile.h"
#include <string>
bool saveSnapshot(const std::string& path, const IdentityProfile& profile, std::string* error=nullptr);
bool restoreSnapshot(const std::string& path, IdentityProfile* profile, std::string* error=nullptr);

