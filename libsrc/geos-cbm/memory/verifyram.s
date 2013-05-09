;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char VerifyRAM (char REUBank, int length, char *reuaddy, char *cpuaddy);

            .import REURegs
            .export _VerifyRAM

            .include "jumptab.inc"
            .include "geossym.inc"

_VerifyRAM:
        jsr REURegs
        jsr VerifyRAM
        txa
        ldx #0
        rts
