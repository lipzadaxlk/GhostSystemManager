#include "../VirtualIdentity/IdentityProfile.h"
#include "../VirtualIdentity/SnapshotManager.h"
#include "../VirtualIdentity/VerificationEngine.h"
#include <cstdio>
#include <iostream>

static std::vector<IdentityObservation> observations(const IdentityProfile& profile, bool useProfile) {
    const std::string suffix = useProfile ? "" : "-before";
    return {{"SMBIOS","System UUID",profile.systemUuid+suffix,"guest WMI/SMBIOS",true},{"BIOS","Vendor",profile.biosVendor,"guest WMI",true},{"BIOS","Version",profile.biosVersion,"guest WMI",true},{"BIOS","Serial",profile.biosSerial,"guest WMI",true},{"Motherboard","Manufacturer",profile.baseboardManufacturer,"guest WMI",true},{"Motherboard","Product",profile.baseboardProduct,"guest WMI",true},{"Motherboard","Serial",profile.baseboardSerial,"guest WMI",true},{"Storage","Identifier",profile.storageId,"guest SetupAPI",true},{"Network","GUID",profile.networkGuid,"guest adapter API",true},{"Network","MAC",profile.macAddress,"guest adapter API",true},{"GPU","Identifier",profile.gpuId,"guest SetupAPI",true},{"Volume","Identifier",profile.volumeId,"guest volume API",true},{"Windows","Installation ID",profile.installationId,"guest registry",true}};
}

int main(){const auto profile=generateIdentityProfile(0x424242ULL); std::string error; const std::string path="identity-flow.snapshot"; if(!saveSnapshot(path,profile,&error)){std::cerr<<error<<"\n";return 1;} IdentityProfile restored; if(!restoreSnapshot(path,&restored,&error)||restored.systemUuid!=profile.systemUuid){std::cerr<<"snapshot round-trip failed\n";return 2;} const auto rows=compareIdentity(profile,observations(profile,false),observations(profile,true)); int changed=0,failed=0; for(const auto& row:rows){if(row.status==ObservationStatus::Alterado)++changed;if(row.status==ObservationStatus::VerificacaoFalhou)++failed;} std::cout<<"rows="<<rows.size()<<" changed="<<changed<<" failed="<<failed<<"\n"; std::remove(path.c_str()); return rows.size()==13&&changed==1&&failed==0?0:3;}

