# Mapa de componentes e evidências

## btbd/hwid — commit `d701113`

- `Kernel/main.c`: dispatches `IOCTL_STORAGE_QUERY_PROPERTY`, `IOCTL_ATA_PASS_THROUGH`, mount manager, NDIS/ARP and NVIDIA queries. Objetivo declarado: substituir respostas de baixo nível. Exige driver/kernel, elevação e compatibilidade com Windows/WDK; risco crítico de BSOD, corrupção e violação de integridade.
- `User/main.c`: sequência de `RegSetValueEx`, exclusões de Registro/arquivos, `vssadmin`, encerramento de WMI. Objetivo: limpar rastros e randomizar IDs; não há restauração transacional.
- `Kernel/Kernel.vcxproj`: KMDF, Windows 10, toolset `WindowsKernelModeDriver10.0`; não é alvo compatível com o CMake/Qt do GHOST.

## semihcevik/hwidspoofer — commit `35a69ad`

- `hwid.cpp`: WMI `ROOT\\CIMV2` para inventário/identificação.
- `spoof/spoof.cpp`: `VanguardDrivers`, encerramento de processos, extração de mapper/driver, execução via shell e exclusão dos artefatos.
- `spoof/cleaner/clean.c`: edição de Registro, escrita de setor, exclusão de arquivos e shadow copies, parada de Winmgmt.
- `client/client.cpp`: socket TCP, reconexão, envio/recepção e criptografia/protocolo sem servidor documentado.
- `spoof/caches.cpp`: alterações de anti-cheat, Windows Update, TPM, Winsock, Firewall e diretórios do sistema.

## NullTerminatorr/Valorant-Spoofer — commit `6fbe430`

- `Source.cpp::main`: chama `AMIDEWINx64.EXE` e `tmac.exe` por `system()`; não valida retorno nem captura estado original.
- `bin/amifldrv64.sys`: driver American Megatrends assinado, porém terceiro e de finalidade firmware.

## sr2echa/Monotone-HWID-Spoofer — commit `86fcc8d`

- `hwid.ps1::Set-HWProfileID/Set-MachineGUID/Set-VolID`: modifica Registro e chama utilitário pré-compilado.
- `Commands/Hidden/process.bat` e `UnbanComplete.vbs`: monitoramento/caixa de mensagem; dependem de executáveis sem fonte.

## GHOST System Manager

- `hardware/HardwareInventory.cpp`: leitura WMI/SMBIOS/SetupAPI/IP Helper/storage/TBS, sem escrita de identidade.
- `network/NetworkConfiguration.cpp`: candidato de `NetworkAddress` dependente do driver, com plano e verificação.
- `backup/BackupStore.cpp` e `logs/OperationLogger.cpp`: persistência local de backup/log de operações.
- `tests/InventoryProbe.cpp`: teste não destrutivo de motherboard, UUID e BIOS; a etapa de TPM agora valida TBS/PNP.

Nenhuma função de terceiro foi copiada. As únicas ideias aproveitadas são consultas de inventário já reimplementadas com interfaces documentadas.
