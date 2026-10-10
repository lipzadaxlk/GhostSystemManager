#include "VirtualBoxBackend.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <algorithm>
#include <chrono>
#include <sstream>
#include <thread>

namespace {
struct CommandResult { bool started=false; unsigned long exitCode=1; std::string output; std::string error; };
std::string narrow(const std::wstring& value){return std::string(value.begin(),value.end());}
std::wstring wide(const std::string& value){return std::wstring(value.begin(),value.end());}
std::wstring quote(const std::string& value){std::wstring result=L"\""; for(const wchar_t ch:wide(value)){if(ch==L'\"')result+=L'\\';result+=ch;} result+=L'\"'; return result;}
CommandResult run(const std::string& executable,const std::vector<std::string>& args){
#ifdef _WIN32
    SECURITY_ATTRIBUTES security{sizeof(security),nullptr,TRUE}; HANDLE readPipe=nullptr,writePipe=nullptr; if(!CreatePipe(&readPipe,&writePipe,&security,0))return{false,1,{},"CreatePipe failed"}; SetHandleInformation(readPipe,HANDLE_FLAG_INHERIT,0);
    std::wstring command=quote(executable); for(const auto& arg:args)command+=L" "+quote(arg); std::vector<wchar_t> commandLine(command.begin(),command.end());commandLine.push_back(L'\0');
    STARTUPINFOW startup{}; startup.cb=sizeof(startup);startup.dwFlags=STARTF_USESTDHANDLES;startup.hStdOutput=writePipe;startup.hStdError=writePipe; PROCESS_INFORMATION process{};
    if(!CreateProcessW(nullptr,commandLine.data(),nullptr,nullptr,TRUE,CREATE_NO_WINDOW,nullptr,nullptr,&startup,&process)){CloseHandle(readPipe);CloseHandle(writePipe);return{false,GetLastError(),{},"CreateProcess failed"};}
    CloseHandle(writePipe); std::string output; char buffer[4096]; DWORD bytes=0; while(ReadFile(readPipe,buffer,sizeof(buffer),&bytes,nullptr)&&bytes)output.append(buffer,bytes); WaitForSingleObject(process.hProcess,INFINITE); DWORD exitCode=1;GetExitCodeProcess(process.hProcess,&exitCode);CloseHandle(process.hThread);CloseHandle(process.hProcess);CloseHandle(readPipe); return{true,exitCode,output,exitCode==0?"":output};
#else
    (void)executable;(void)args;return{false,1,{},"Windows only"};
#endif
}
std::string trim(std::string value){while(!value.empty()&&(value.back()=='\r'||value.back()=='\n'||value.back()==' '))value.pop_back();return value;}
}

VirtualBoxStatus detectVirtualBox(){
#ifdef _WIN32
    wchar_t path[MAX_PATH]{}; DWORD length=SearchPathW(nullptr,L"VBoxManage.exe",nullptr,MAX_PATH,path,nullptr); if(length&&length<MAX_PATH)return{true,narrow(std::wstring(path,length)),"VBoxManage detected in PATH."};
    HKEY key=nullptr; wchar_t install[MAX_PATH]{}; DWORD size=sizeof(install); if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Oracle\\VirtualBox",0,KEY_READ|KEY_WOW64_64KEY,&key)==ERROR_SUCCESS){if(RegQueryValueExW(key,L"InstallDir",nullptr,nullptr,reinterpret_cast<BYTE*>(install),&size)==ERROR_SUCCESS){RegCloseKey(key); std::wstring exe=std::wstring(install)+L"VBoxManage.exe"; if(GetFileAttributesW(exe.c_str())!=INVALID_FILE_ATTRIBUTES)return{true,narrow(exe),"VirtualBox detected from registry."};}RegCloseKey(key);}
#endif
    return{false,{},"VirtualBox is not installed or VBoxManage.exe is not accessible."};
}

std::vector<VirtualBoxVm> listVirtualBoxVms(const VirtualBoxStatus& status,std::string* error){
    std::vector<VirtualBoxVm> result; if(!status.installed){if(error)*error=status.detail;return result;} const auto command=run(status.executable,{"list","vms"}); if(!command.started||command.exitCode!=0){if(error)*error=command.error;return result;}
    std::istringstream input(command.output); std::string line; while(std::getline(input,line)){const auto open=line.find('"'),close=line.find('"',open+1),brace=line.find('{',close); if(open==std::string::npos||close==std::string::npos||brace==std::string::npos)continue; const auto end=line.find('}',brace);if(end==std::string::npos)continue;result.push_back({line.substr(open+1,close-open-1),line.substr(brace+1,end-brace-1),"unknown"});} return result;
}

VirtualBoxApplyResult applyVirtualBoxIdentity(const VirtualBoxStatus& status,const VirtualBoxVm& vm,const IdentityProfile& profile,bool restartVm){
    VirtualBoxApplyResult result; if(!status.installed){result.detail=status.detail;return result;} if(vm.uuid.empty()){result.detail="A VM existente deve ser selecionada.";return result;}
    std::string error; const auto info=run(status.executable,{"showvminfo",vm.uuid,"--machinereadable"}); if(!info.started||info.exitCode!=0){result.detail="Não foi possível consultar o estado da VM: "+info.error;return result;} const bool running=info.output.find("VMState=\"running\"")!=std::string::npos;
    if(running){if(!restartVm){result.rebootRequired=true;result.detail="A VM está em execução; reinicialização é necessária.";return result;} const auto stop=run(status.executable,{"controlvm",vm.uuid,"acpipowerbutton"});if(!stop.started||stop.exitCode!=0){result.detail="Não foi possível solicitar desligamento ACPI da VM.";return result;} for(int attempt=0;attempt<30;++attempt){std::this_thread::sleep_for(std::chrono::seconds(1));const auto state=run(status.executable,{"showvminfo",vm.uuid,"--machinereadable"});if(state.output.find("VMState=\"poweroff\"")!=std::string::npos)break;}}
    const auto modify=run(status.executable,{"modifyvm",vm.uuid,"--hardwareuuid",profile.systemUuid,"--macaddress1",profile.macAddress}); if(!modify.started||modify.exitCode!=0){result.detail="VBoxManage rejeitou a configuração: "+modify.error;return result;} result.applied=true; result.configured={"System UUID virtual (hardwareuuid)","MAC do adaptador 1"}; result.unsupported={"BIOS vendor/version/serial","Baseboard manufacturer/product/serial","Storage serial","GPU identity","Volume identifier","Windows installation identifier","TPM identity"}; result.rebootRequired=running;
    if(running){const auto start=run(status.executable,{"startvm",vm.uuid,"--type","headless"});if(!start.started||start.exitCode!=0){result.detail="Identificadores aplicados, mas a VM não iniciou: "+start.error;return result;}}
    result.success=false; result.guestVerified=false; result.detail="VirtualBox aplicou hardwareuuid e MAC, mas a verificação independente dentro do convidado requer Guest Additions/credenciais e permanece pendente."; return result;
}

