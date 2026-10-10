# HWIDResearch

Pesquisa isolada de identidade virtual sintética e comunicação user-mode/KMDF.

## Limites de segurança

O driver não altera SMBIOS, UUID, discos, MAC, firmware, Registro ou estruturas internas do Windows. O IOCTL retorna apenas diagnósticos e textos sintéticos. O loader não instala nada por padrão e não contém mapper manual, BYOVD, exploração, bypass de assinatura ou interação com anti-cheat.

## Perfis virtuais e verificação

`HWIDResearchIdentityFlowProbe.exe` gera um perfil persistente com UUID, BIOS, baseboard, armazenamento, rede, GPU, volume e instalação. O perfil é um artefato de teste; ele não é injetado em processos nem substitui a identidade do host. A comparação recebe observações coletadas no convidado por uma fonte identificada e retorna `ALTERADO`, `NAO ALTERADO`, `NAO SUPORTADO` ou `VERIFICACAO FALHOU`. Um valor armazenado pelo próprio perfil nunca é considerado observação.

O hipervisor deve ser configurado explicitamente pelo operador, usando somente sua documentação. Exemplos de preparação em uma VM descartável: gerar um novo UUID da VM, criar um novo disco virtual, gerar um novo MAC virtual e instalar o Windows novamente. O projeto não executa esses comandos automaticamente e não altera a VM do host.

## Estado do ambiente

O computador de desenvolvimento possui Windows SDK, MSVC e CMake, mas não possui o WDK/KMDF instalado. Por isso, o código e o projeto `Driver/HWIDResearch.vcxproj` estão preparados para compilação em uma VM de testes com WDK, enquanto os componentes user-mode são compiláveis localmente.

## Compilar user-mode

```powershell
cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
build\HWIDResearchVirtualIdentityProbe.exe
build\HWIDResearchHardwareProbe.exe
build\HWIDResearchLoader.exe
```

## Driver em VM de testes

1. Instale o WDK correspondente ao SDK/Visual Studio.
2. Abra o projeto `.vcxproj` no Visual Studio x64.
3. Compile em Debug com certificado de teste do ambiente de laboratório.
4. Ative test-signing somente na VM descartável, reinicie e confirme o estado com `bcdedit /enum`.
5. Instale o `.sys` usando o SCM ou o deployment do Visual Studio; não use mapeamento manual.
6. Execute `HWIDResearchLoader.exe --status` para consultar a interface.
7. Pare/remova o serviço pelo SCM após o teste.

## WinDbg

Use WinDbg Preview na VM, habilite símbolos Microsoft (`srv*C:\symbols*https://msdl.microsoft.com/download/symbols`), conecte kernel debugging antes de carregar o driver e verifique `!analyze -v`, `lm`, `!devstack` e os eventos WPP. Não depure o driver em um computador com dados pessoais.

