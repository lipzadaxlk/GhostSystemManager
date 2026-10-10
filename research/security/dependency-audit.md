# Auditoria de dependências

| Repositório | Dependências detectadas | Verificação | Risco |
|---|---|---|---|
| NullTerminatorr | AMIDEWINx64, amifldrv64.sys, TMAC; runtimes MSVC/VB6 | binários pré-compilados; três artefatos sem assinatura | Alto; firmware/driver de terceiro |
| btbd | Windows SDK/WDK, KMDF, MSVC v142 | projeto antigo, sem pacote reprodutível; arquivado | Alto; driver/kernel |
| Monotone | EXEs utilitários, driver, PowerShell, VBS, batch | fontes ausentes para artefatos | Crítico; cadeia não auditável |
| semihcevik | MSVC v142, DirectX9, Dear ImGui vendored, driver/mapper embutidos, socket TCP | ImGui identificável; mapper/driver e servidor não confiáveis | Crítico |

O GHOST usa apenas MSVC, CMake, Qt 6, Windows SDK e APIs documentadas. Nenhuma dependência desses repositórios foi adicionada.

Inventário SHA-256 dos binários: `research/security/binary-inventory.csv`.
