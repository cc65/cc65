;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char SaveFile  (char skip, struct fileheader *myHeader);

            .import setoserror
            .import popa
            .export _SaveFile

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"

_SaveFile:
        sta r9L
        stx r9H
        jsr popa
        sta r10L
        jsr SaveFile
        jmp setoserror
