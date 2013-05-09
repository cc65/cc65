;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char GetBlock (struct tr_se *myTS, char *buffer);

            .import popax, setoserror
            .import gettrse
            .export _GetBlock

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_GetBlock:
        sta r4L
        stx r4H
        jsr popax
        jsr gettrse
        sta r1L
        stx r1H
        jsr GetBlock
        jmp setoserror
