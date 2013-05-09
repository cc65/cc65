;
; Maciej 'YTM/Alliance' Witkowiak
;
; 27.10.99

; void  InitProcesses   (char number, struct process* proctab);
; (rest)
; void  BlockProcess    (char number);
; void  UnblockProcess  (char number);

            .export _BlockProcess
            .export _UnblockProcess

            .include "jumptab.inc"
            
_BlockProcess:
        tax
        jmp BlockProcess
            
_UnblockProcess:
        tax
        jmp UnblockProcess
