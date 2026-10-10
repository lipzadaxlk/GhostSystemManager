# Persistência e restauração

## Operações legitimamente integráveis

1. Nome do computador: API documentada (`SetComputerNameEx`) com elevação, backup do nome anterior, confirmação e verificação; reinicialização pode ser necessária e nunca é automática.
2. Propriedade `NetworkAddress`: somente se o adaptador/driver expuser essa propriedade documentada. Salvar o valor original, aplicar após confirmação, consultar novamente e oferecer restauração.
3. Perfil de VM: delegar ao hypervisor (Hyper-V/VMware/VirtualBox) e comparar instalações; não modificar firmware/hardware do host.

## Operações rejeitadas

- Escrita de SMBIOS, UUID, serial de placa, BIOS, CPU, GPU ou TPM.
- Escrita direta de setor de volume, `IOCTL` interceptado, hook de kernel ou driver mapeado.
- Randomização de `MachineGuid`, ProductId, IDs de ativação, Windows Update, TPM, caches e registros de anti-cheat.
- Exclusão de shadow copies, logs, `setupapi.dev.log`, Prefetch, `C:\Windows\INF`, perfis de usuário ou arquivos de jogos.

Essas operações não têm restauração confiável, podem corromper o sistema, quebrar licenciamento/Windows Update e ultrapassam o escopo de configuração suportada.
