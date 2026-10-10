#include "VerificationEngine.h"
#include <map>

namespace {
using Map=std::map<std::string,IdentityObservation>;
Map index(const std::vector<IdentityObservation>& values){Map result;for(const auto& item:values)result[item.category+"/"+item.name]=item;return result;}
std::vector<std::pair<std::string,std::string>> expectedFields(const IdentityProfile& p){return {{"SMBIOS/System UUID",p.systemUuid},{"BIOS/Vendor",p.biosVendor},{"BIOS/Version",p.biosVersion},{"BIOS/Serial",p.biosSerial},{"Motherboard/Manufacturer",p.baseboardManufacturer},{"Motherboard/Product",p.baseboardProduct},{"Motherboard/Serial",p.baseboardSerial},{"Storage/Identifier",p.storageId},{"Network/GUID",p.networkGuid},{"Network/MAC",p.macAddress},{"GPU/Identifier",p.gpuId},{"Volume/Identifier",p.volumeId},{"Windows/Installation ID",p.installationId}};}
}

std::vector<ComparisonRow> compareIdentity(const IdentityProfile& expected, const std::vector<IdentityObservation>& before, const std::vector<IdentityObservation>& after){
    const auto oldValues=index(before); const auto newValues=index(after); std::vector<ComparisonRow> rows;
    for(const auto& field:expectedFields(expected)){const auto oldIt=oldValues.find(field.first), newIt=newValues.find(field.first); ComparisonRow row{}; const auto slash=field.first.find('/'); row.category=field.first.substr(0,slash); row.name=field.first.substr(slash+1); row.expected=field.second;
        if(newIt==newValues.end()){row.status=ObservationStatus::NaoSuportado;row.source="not observed in guest";}
        else if(!newIt->second.supported){row.status=ObservationStatus::NaoSuportado;row.observed=newIt->second.value;row.source=newIt->second.source;}
        else {row.observed=newIt->second.value;row.source=newIt->second.source;row.previous=oldIt==oldValues.end()?"":oldIt->second.value;row.status=row.observed==row.expected ? (row.previous==row.observed?ObservationStatus::NaoAlterado:ObservationStatus::Alterado) : ObservationStatus::VerificacaoFalhou;}
        rows.push_back(std::move(row)); }
    return rows;
}

const char* observationStatusText(ObservationStatus status){switch(status){case ObservationStatus::Alterado:return "ALTERADO";case ObservationStatus::NaoAlterado:return "NAO ALTERADO";case ObservationStatus::NaoSuportado:return "NAO SUPORTADO";default:return "VERIFICACAO FALHOU";}}

