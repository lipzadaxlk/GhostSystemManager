# Auditoria de repositórios — Ghost System Manager

Data da coleta: 2026-10-09. A análise foi estática; nenhum binário, script, driver ou instalador dos repositórios foi executado.

## Fontes e legitimidade

| Fonte | Estado | Branch/commit observado | Última alteração observada | Licença | Resultado |
|---|---|---|---|---|---|
| `NullTerminatorr/Valorant-Spoofer` | GitHub público; contém binários | `main`, `6fbe43015424ee402a93ab11e98320f02f1ba6e4` | 2022-10-02 | Nenhum arquivo de licença encontrado | Rejeitado: executa AMIDEWIN e TMAC para alterar firmware/MAC; binários sem assinatura |
| `btbd/hwid` | GitHub público, arquivado em 2021-04-04 | `master`, `d701113593dd7346c3e834e82004a0f241b40e57` | 2020-06-27 | Nenhum arquivo de licença encontrado | Rejeitado: driver KMDF e código explicitamente descrito pelo autor como apressado |
| `sr2echa/Monotone-HWID-Spoofer` | GitHub público; distribui muitos binários | `master`, `86fcc8d344abf8418a57edc75e1ee16f8225f6e1` | 2022-05-18 | Nenhum arquivo de licença encontrado | Rejeitado: scripts/driver/binários não auditáveis e alteração de volume/identidade |
| `semihcevik/hwidspoofer` | GitHub público; fork/adaptação declarada de `btbd/hwid` | `main`, `35a69ad784f0083010c648805b0e6238ff168ec6` | 2025-05-18 | Nenhum arquivo de licença encontrado | Rejeitado: driver, mapper, limpeza destrutiva, comandos de anti-cheat e cliente TCP |
| `semihcevik/hwidspooferestude` | GitHub retorna 404 | — | — | — | Não substituído por outro projeto; endereço registrado como incorreto/inacessível |
| `sites.google.com/view/github-launcher` | Google Sites, não é repositório verificável | — | página atualizada sem data precisa | Não declarada | Não confiável; instrui desligar antivírus antes de baixar EasyLauncher; não baixado |

## Arquiteturas encontradas

- `btbd/hwid`: user mode C e driver KMDF C; intercepta consultas de disco, volume, NIC/ARP, SMBIOS e GPU, além de editar Registro e arquivos.
- `semihcevik/hwidspoofer`: user mode C/C++ com Dear ImGui, WMI, cliente TCP, componentes embutidos, mapper/driver e cleaner; README credita o driver a `btbd/hwid`.
- `NullTerminatorr/Valorant-Spoofer`: pequeno launcher C++ que chama executáveis de terceiros `AMIDEWINx64.EXE` e `tmac.exe` via `system()`.
- `Monotone`: executável e scripts batch/PowerShell/VBS, driver e utilitários pré-compilados; não há fonte correspondente para vários artefatos.

## Decisão

Nenhum código foi copiado para o produto. O aproveitamento aprovado limita-se a ideias de inventário já implementadas de forma independente com Win32/WMI/SMBIOS/TBS. Drivers, mappers, spoofing, limpeza de rastros, bypass de proteções e executáveis de terceiros foram rejeitados.
