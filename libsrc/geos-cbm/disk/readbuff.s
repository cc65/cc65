;
; Maciej 'YTM/Elysium' Witkowiak
;
; 26.10.1999, 2.1.2003

; char ReadBuff  (struct tr_se);

            .export _ReadBuff
            .import setoserror
            .import gettrse

            .include "diskdrv.inc"
            .include "geossym.inc"
        
_ReadBuff:
        jsr gettrse
        sta r1L
        stx r1H
        jsr ReadBuff
        jmp setoserror
