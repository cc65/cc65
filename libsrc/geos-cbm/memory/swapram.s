;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; void SwapRAM (char REUBank, int length, char *reuaddy, char *cpuaddy);
;       note that in all REU procs last two pointers are identified like here

            .import REURegs
            .export _SwapRAM

            .include "jumptab.inc"
            .include "geossym.inc"

_SwapRAM:
        jsr REURegs
        jmp SwapRAM
