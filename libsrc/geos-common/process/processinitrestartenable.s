;
; Maciej 'YTM/Alliance' Witkowiak
;
; 27.10.99

; void  InitProcesses   (char number, struct process* proctab);
; void  RestartProcess  (char number);
; void  EnableProcess   (char number);

            .import popa
            .export _InitProcesses
            .export _RestartProcess
            .export _EnableProcess

            .include "jumptab.inc"
            .include "geossym.inc"

_InitProcesses:
        sta r0L
        stx r0H
        jsr popa
        jmp InitProcesses
            
_RestartProcess:
        tax
        jmp RestartProcess
            
_EnableProcess:
        tax
        jmp EnableProcess
