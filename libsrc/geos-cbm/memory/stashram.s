;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void StashRAM (char REUBank, int length, char *dest, char *from);

            .import REURegs
            .export _StashRAM

            .include "jumptab.inc"
            .include "geossym.inc"

_StashRAM:
        jsr REURegs
        jmp StashRAM
