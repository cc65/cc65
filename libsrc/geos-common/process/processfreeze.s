;
; Maciej 'YTM/Alliance' Witkowiak
;
; 27.10.99

; void  FreezeProcess   (char number);
; void  UnfreezeProcess (char number);

            .export _FreezeProcess
            .export _UnfreezeProcess

            .include "jumptab.inc"
            
_FreezeProcess:
        tax
        jmp FreezeProcess
            
_UnfreezeProcess:
        tax
        jmp UnfreezeProcess
