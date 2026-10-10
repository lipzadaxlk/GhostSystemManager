# Compatibilidade em computadores físicos

| Categoria | Origem consultada | Alteração suportada pelo projeto | Verificação |
|---|---|---|---|
| SMBIOS/UUID | WMI `Win32_ComputerSystemProduct`, SMBIOS `RSMB` | somente leitura | duas fontes quando disponíveis |
| Placa-mãe/BIOS | WMI `Win32_BaseBoard` e `Win32_BIOS` | somente leitura; firmware exige fabricante | WMI/SMBIOS |
| Discos | `IOCTL_STORAGE_QUERY_PROPERTY` | somente leitura | consulta de dispositivo |
| Volumes | APIs de volume | somente leitura | enumeração local |
| Rede/MAC | `GetAdaptersAddresses` e propriedade `NetworkAddress` do driver | configurável somente se o driver expuser a propriedade | MAC ativo via `GetAdaptersAddresses` |
| GPU | SetupAPI/WMI | somente leitura | enumeração de dispositivo |
| TPM | TBS/WMI quando disponível | somente leitura | presença/estado reportados separadamente |
| Windows MachineGuid/ProductId | Registro/WMI | somente leitura no projeto | segunda leitura do Registro/WMI |
| Nome do computador | `SetComputerNameExW` | configuração suportada, pode exigir reinicialização | `GetComputerNameExW` |

O botão `APPLY IDENTITY` aplica somente a configuração lógica de rede que passou pela checagem de capacidade. Não altera firmware, TPM, UUID físico, serial de disco ou identificadores de instalação. Se o MAC ativo não coincidir após a aplicação, o resultado é falha/parcial.

