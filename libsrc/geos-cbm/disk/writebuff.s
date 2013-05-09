;
; Maciej 'YTM/Elysium' Witkowiak
;
; 26.10.1999, 2.1.2003

; char WriteBuff (struct tr_se*);

            .export _WriteBuff
            .import setoserror
            .import gettrse

            .include "diskdrv.inc"
            .include "geossym.inc"
        
_WriteBuff:
        jsr gettrse
        sta r1L
        stx r1H
        jsr WriteBuff
        jmp setoserror
