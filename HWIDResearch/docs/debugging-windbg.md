# Depuração WinDbg

O driver é KMDF e deve ser testado somente em VM. Use kernel debugging configurado pelo Visual Studio/WinDbg, valide o certificado de teste e colete um dump antes de qualquer alteração. Comandos mínimos: `!analyze -v`, `lmvm HWIDResearch`, `!wdfkd.wdfdevice`, `!wdfkd.wdfqueue` e `ed nt!Kd_Default_Mask 8` quando necessário para diagnóstico. Não desative Secure Boot ou Defender no host.

