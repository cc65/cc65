;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char FreeFile  (struct trse myTrSe[]);

            .import setoserror
            .export _FreeFile

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_FreeFile:
        sta r9L
        stx r9H
        jsr FreeFile
        jmp setoserror
