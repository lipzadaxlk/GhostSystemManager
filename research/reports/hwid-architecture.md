# Arquitetura de identificadores observada

| Identificador | Origem real / API | Classificação para o GHOST | Evidência dos repositórios | Decisão |
|---|---|---|---|---|
| SMBIOS/BIOS/baseboard | `GetSystemFirmwareTable('RSMB')`, WMI, registro | FIRMWARE CONTROLLED / READ ONLY | `btbd` remove/edita dados SMBIOS; `semihcevik` reutiliza | Somente leitura e validação independente |
| System UUID | SMBIOS Type 1 / `Win32_ComputerSystemProduct.UUID` | FIRMWARE CONTROLLED / READ ONLY | projetos tentam alterar cache/SMBIOS, sem garantia física | Somente leitura; não fabricar valor |
| Motherboard/BIOS serial | SMBIOS Type 2/0, WMI | FIRMWARE CONTROLLED / READ ONLY | AMIDEWIN e hooks de SMBIOS | Somente leitura |
| CPU | CPUID/registro/WMI | READ ONLY | alterações propostas são hooks/driver | Somente leitura |
| GPU | Enumeração de display, registro do driver, EDID | READ ONLY | `btbd`/`semihcevik` mexem em cache/EDID | Somente leitura |
| Disk serial | `IOCTL_STORAGE_QUERY_PROPERTY`, ATA/NVMe | READ ONLY | drivers interceptam IOCTLs; README admite NVMe não tratado | Somente leitura |
| Volume serial | `GetVolumeInformation`, metadados NTFS/FAT | READ ONLY; alteração de baixo nível UNSUPPORTED/HIGH RISK | `Monotone` chama Volumeid; `semihcevik` escreve setor | Não integrar escrita |
| MAC/NetworkAddress | NDIS/driver; configuração `NetworkAddress` depende do driver | SUPPORTED CONFIGURATION apenas quando o driver documenta | `NullTerminatorr` usa TMAC; outros editam Registro | Manter apenas opção explícita, backup, confirmação e verificação |
| MachineGuid e IDs Windows | Registro e APIs de instalação | READ ONLY | projetos randomizam/deletam chaves | Não alterar |
| TPM | PNP, TBS, WMI `ROOT\\CIMV2\\Security\\MicrosoftTpm` | READ ONLY | projetos tentam alterar `WindowsAIKHash`/ODUID | Somente inspeção |

Cada valor deve ser acompanhado pela fonte, privilégio, erro e segunda fonte independente. Ausência de uma fonte não autoriza inferência de valor.
