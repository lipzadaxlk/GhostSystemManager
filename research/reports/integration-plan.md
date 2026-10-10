# Plano de integração aprovado

## Aprovado

- `HardwareInventory`: manter consultas WMI/COM, SMBIOS via `GetSystemFirmwareTable`, PNP/TBS para TPM, SetupAPI, IP Helper e storage IOCTL de leitura.
- `CompatibilityChecker`: classificar cada registro como READ ONLY ou SUPPORTED CONFIGURATION com razão e API.
- `BackupManager`/`VerificationEngine`: exigir captura anterior, confirmação, aplicação mínima, reconsulta independente e restauração.
- `SecurityAudit`: registrar origem, assinatura, hash e bloqueio de componentes de terceiros.
- `OperationLogger`: registrar sucesso somente após verificação.

## Pendências seguras

- Separar `SMBIOSReader`, `SystemUUIDReader` e `TPMInspector` em interfaces próprias.
- Adicionar teste que compara SMBIOS, WMI e registro para UUID.
- Adicionar teste de TPM que distingue WMI negado, TBS presente, dispositivo PNP presente e ausência real.
- Implementar somente renomeação de computador e configuração de MAC quando o driver declarar suporte.

## Rejeitado

Qualquer driver de terceiros, mapper, AMIDEWIN, TMAC, Volumeid, alteração de TPM/SMBIOS, limpeza de rastros, bypass de Riot/Vanguard/EAC/BattlEye, desativação de Secure Boot/HVCI/Defender ou download dinâmico.
