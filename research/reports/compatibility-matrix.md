# Matriz comparativa

| Projeto | Arquitetura | HWIDs | Persistência declarada | Compatibilidade declarada | Segurança | Código reutilizado | Integração |
|---|---|---|---|---|---|---|---|
| NullTerminatorr | User mode + drivers/EXEs AMI/TMAC | SMBIOS/BIOS, MAC | firmware/driver-dependent | sem versão precisa | binários sem assinatura; firmware writer | não evidenciado | Rejeitada |
| btbd/hwid | Kernel KMDF + user C | disco, volume, NIC/ARP, SMBIOS, boot, GPU, Registro | mista/desconhecida | Win10 1507/1809/1903 x64 | arquivado; código apressado; apaga rastros | base de `semihcevik` | Rejeitada |
| Monotone | EXE + batch/PS1/VBS + driver | volume, MAC, IDs Windows | parcialmente temporária | não especificada | muitos binários não auditados; driver sem assinatura válida | não evidenciado | Rejeitada |
| semihcevik | ImGui user + mapper + kernel + cleaner + TCP | disco, volume, NIC, SMBIOS, boot, GPU, caches | desconhecida/mista | Win10 antigo declarado | desativa proteções, apaga dados, anti-cheat, rede | credita btbd | Rejeitada |
| Google launcher | launcher desconhecido | desconhecido | desconhecida | desconhecida | pede desligar antivírus; download externo | desconhecido | Rejeitada |

Popularidade, estrelas ou alegações de “undetected” não são evidência técnica.
