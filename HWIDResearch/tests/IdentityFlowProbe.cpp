#include "../VirtualIdentity/IdentityProfile.h"
#include "../VirtualIdentity/SnapshotManager.h"
#include "../VirtualIdentity/VerificationEngine.h"
#include <cstdio>
#include <iostream>

static std::vector<IdentityObservation> observations(const IdentityProfile& profile, bool useProfile, bool independent) {
    const std::string suffix = useProfile ? "" : "-before";
    return {{"SMBIOS","System UUID",profile.systemUuid+suffix,"fixture only",true,independent},{"BIOS","Vendor",profile.biosVendor,"fixture only",true,independent},{"BIOS","Version",profile.biosVersion,"fixture only",true,independent},{"BIOS","Serial",profile.biosSerial,"fixture only",true,independent},{"Motherboard","Manufacturer",profile.baseboardManufacturer,"fixture only",true,independent},{"Motherboard","Product",profile.baseboardProduct,"fixture only",true,independent},{"Motherboard","Serial",profile.baseboardSerial,"fixture only",true,independent},{"Storage","Identifier",profile.storageId,"fixture only",true,independent},{"Network","GUID",profile.networkGuid,"fixture only",true,independent},{"Network","MAC",profile.macAddress,"fixture only",true,independent},{"GPU","Identifier",profile.gpuId,"fixture only",true,independent},{"Volume","Identifier",profile.volumeId,"fixture only",true,independent},{"Windows","Installation ID",profile.installationId,"fixture only",true,independent}};
}

int main(){const auto profile=generateIdentityProfile(0x424242ULL); std::string error; const std::string path="identity-flow.snapshot"; if(!saveSnapshot(path,profile,&error)){std::cerr<<error<<"\n";return 1;} IdentityProfile restored; if(!restoreSnapshot(path,&restored,&error)||restored.systemUuid!=profile.systemUuid){std::cerr<<"snapshot round-trip failed\n";return 2;} const auto rows=compareIdentity(profile,observations(profile,false,false),observations(profile,true,false)); int changed=0,failed=0; for(const auto& row:rows){if(row.status==ObservationStatus::Alterado)++changed;if(row.status==ObservationStatus::VerificacaoFalhou)++failed;} std::cout<<"rows="<<rows.size()<<" changed="<<changed<<" failed="<<failed<<" (fixture rejected as non-independent)\n"; std::remove(path.c_str()); return rows.size()==13&&changed==0&&failed==13?0:3;}

