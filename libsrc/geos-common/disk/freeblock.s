;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char FreeBlock (struct tr_se *TS);

            .import gettrse, setoserror
            .export _FreeBlock

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_FreeBlock:
        jsr gettrse
        sta r6L
        stx r6H
        jsr FreeBlock
        jmp setoserror
