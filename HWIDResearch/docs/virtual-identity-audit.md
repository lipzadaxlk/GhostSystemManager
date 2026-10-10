# Auditoria da identidade virtual

## Estado comprovado neste ambiente

O probe `HWIDResearchHypervisorProbe` retornou `backend=None`. Não foram encontrados `VBoxManage.exe`, `vmrun.exe`, `qemu-system-x86_64.exe` nem cmdlets Hyper-V disponíveis. Portanto nenhuma VM foi modificada.

## Auditoria dos 13 campos

| Campo | Consulta independente atual | Estado |
|---|---|---|
| System UUID | Não no `IdentityFlowProbe`; o leitor de hardware só confirma disponibilidade da tabela SMBIOS | não comprovado |
| BIOS vendor/version/serial | Não implementado no probe de identidade | não suportado neste fluxo |
| Baseboard manufacturer/product/serial | Não implementado no probe de identidade | não suportado neste fluxo |
| Storage identifier | O leitor atual enumera volumes; não altera nem afirma serial virtual | somente leitura |
| Network GUID/MAC | O leitor atual enumera adaptadores; não configura sem backend/VM | somente leitura |
| GPU identifier | O módulo atual é marcador de limite e não consulta uma GPU virtual | não suportado |
| Volume identifier | Não existe aplicação de volume virtual | não suportado |
| Windows installation identifier | Não existe aplicação de instalação | somente leitura |

O resultado antigo `rows=13 changed=1 failed=0` era produzido pelo fixture de teste: ele concatenava `-before` ao UUID em memória e fornecia os demais valores diretamente do perfil. Não representava uma mudança no Windows. O teste foi corrigido; agora fixtures não independentes produzem `changed=0 failed=13`.

## O que pode ser configurado legitimamente

Somente o hypervisor, usando a configuração documentada da VM, pode fornecer UUID/BIOS/baseboard/disco/MAC virtuais ao convidado. Isso exige um alvo explícito e reinicialização quando o hypervisor solicitar. TPM e identidade física do host permanecem fora do escopo. O VerificationEngine só pode retornar `ALTERADO` quando a observação pós-operação tiver `independentlyObserved=true` e origem documentada do convidado.

