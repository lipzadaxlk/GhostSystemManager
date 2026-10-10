#include "SnapshotManager.h"
bool saveSnapshot(const std::string& path,const IdentityProfile& profile,std::string* error){return saveIdentityProfile(path,profile,error);}
bool restoreSnapshot(const std::string& path,IdentityProfile* profile,std::string* error){return loadIdentityProfile(path,profile,error);}

