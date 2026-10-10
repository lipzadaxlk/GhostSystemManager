#include "IdentityProfile.h"
#include "IdentityGenerator.h"
#include <array>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace {
std::string hex(std::uint64_t value) { std::ostringstream out; out << std::uppercase << std::hex << std::setfill('0') << std::setw(16) << value; return out.str(); }
std::string escape(std::string value) { for (size_t pos=0; (pos=value.find('\\',pos))!=std::string::npos; pos+=2) value.replace(pos,1,"\\\\"); for(size_t pos=0;(pos=value.find('\n',pos))!=std::string::npos;pos+=2)value.replace(pos,1,"\\n"); return value; }
std::string unescape(std::string value) { std::string out; for(size_t i=0;i<value.size();++i){ if(value[i]=='\\' && i+1<value.size()){ if(value[i+1]=='n'){out+='\n';++i;continue;} if(value[i+1]=='\\'){out+='\\';++i;continue;} } out+=value[i]; } return out; }
bool hexDigit(char value) { return std::isxdigit(static_cast<unsigned char>(value)) != 0; }
bool uuidFormat(const std::string& value) { if(value.size()!=36)return false; for(size_t i=0;i<value.size();++i) if((i==8||i==13||i==18||i==23)?value[i]!='-':!hexDigit(value[i])) return false; return true; }
bool macFormat(const std::string& value) { if(value.size()!=17)return false; for(size_t i=0;i<value.size();++i) if(i%3==2?value[i]!=':':!hexDigit(value[i])) return false; return true; }
}

IdentityProfile generateIdentityProfile(std::uint64_t seed) {
    const auto base=generateSyntheticIdentity(seed);
    IdentityProfile profile;
    profile.profileId="virtual-"+hex(seed);
    profile.systemUuid=base.systemUuid; profile.biosVendor="Ghost Research Firmware"; profile.biosVersion="VIRT-1.0"; profile.biosSerial="BIOS-"+hex(seed ^ 0xB105F00DULL);
    profile.baseboardManufacturer="Ghost Research Systems"; profile.baseboardProduct="Virtual Test Board"; profile.baseboardSerial="BOARD-"+hex(seed ^ 0xBA5EBA11ULL);
    profile.storageId=base.diskId; profile.networkGuid="{"+base.systemUuid+"}"; profile.macAddress=base.macAddress; profile.gpuId="VIRTGPU-"+hex(seed ^ 0x6E766964ULL); profile.volumeId="VOL-"+hex(seed ^ 0xC011EC7AULL).substr(0,8); profile.installationId="INSTALL-"+hex(seed ^ 0x1A57A11ULL);
    return profile;
}

bool validateIdentityProfile(const IdentityProfile& profile, std::string* error) {
    const auto fail=[&](const char* text){if(error)*error=text;return false;};
    if(profile.profileId.empty()||!uuidFormat(profile.systemUuid)||!macFormat(profile.macAddress)) return fail("profile id, UUID or MAC format is invalid");
    if(profile.biosVendor.empty()||profile.biosVersion.empty()||profile.biosSerial.empty()||profile.baseboardManufacturer.empty()||profile.baseboardProduct.empty()||profile.baseboardSerial.empty()) return fail("firmware or baseboard fields are incomplete");
    if(profile.storageId.empty()||profile.networkGuid.empty()||profile.gpuId.empty()||profile.volumeId.empty()||profile.installationId.empty()) return fail("virtual identity fields are incomplete");
    return true;
}

bool saveIdentityProfile(const std::string& path, const IdentityProfile& profile, std::string* error) {
    if(!validateIdentityProfile(profile,error)) return false;
    const std::string temp=path+".tmp"; std::ofstream file(temp,std::ios::binary|std::ios::trunc); if(!file){if(error)*error="cannot create profile file";return false;}
    const std::array<std::pair<const char*,const std::string*>,14> fields={{{"profileId",&profile.profileId},{"systemUuid",&profile.systemUuid},{"biosVendor",&profile.biosVendor},{"biosVersion",&profile.biosVersion},{"biosSerial",&profile.biosSerial},{"baseboardManufacturer",&profile.baseboardManufacturer},{"baseboardProduct",&profile.baseboardProduct},{"baseboardSerial",&profile.baseboardSerial},{"storageId",&profile.storageId},{"networkGuid",&profile.networkGuid},{"macAddress",&profile.macAddress},{"gpuId",&profile.gpuId},{"volumeId",&profile.volumeId},{"installationId",&profile.installationId}}};
    for(const auto& field:fields) file<<field.first<<"="<<escape(*field.second)<<"\n"; file.close(); if(!file) {if(error)*error="cannot write profile file";return false;}
    std::error_code ec; std::filesystem::remove(path,ec); std::filesystem::rename(temp,path,ec); if(ec){if(error)*error=ec.message();return false;} return true;
}

bool loadIdentityProfile(const std::string& path, IdentityProfile* profile, std::string* error) {
    if(!profile){if(error)*error="profile output is null";return false;} std::ifstream file(path,std::ios::binary); if(!file){if(error)*error="cannot open profile file";return false;}
    IdentityProfile loaded; std::string line; while(std::getline(file,line)){const auto sep=line.find('='); if(sep==std::string::npos) continue; const auto key=line.substr(0,sep); const auto value=unescape(line.substr(sep+1));
        if(key=="profileId")loaded.profileId=value; else if(key=="systemUuid")loaded.systemUuid=value; else if(key=="biosVendor")loaded.biosVendor=value; else if(key=="biosVersion")loaded.biosVersion=value; else if(key=="biosSerial")loaded.biosSerial=value; else if(key=="baseboardManufacturer")loaded.baseboardManufacturer=value; else if(key=="baseboardProduct")loaded.baseboardProduct=value; else if(key=="baseboardSerial")loaded.baseboardSerial=value; else if(key=="storageId")loaded.storageId=value; else if(key=="networkGuid")loaded.networkGuid=value; else if(key=="macAddress")loaded.macAddress=value; else if(key=="gpuId")loaded.gpuId=value; else if(key=="volumeId")loaded.volumeId=value; else if(key=="installationId")loaded.installationId=value; }
    if(!validateIdentityProfile(loaded,error)) return false; *profile=std::move(loaded); return true;
}

