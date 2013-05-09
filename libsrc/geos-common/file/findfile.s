;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char FindFile  (char *myName);

            .import setoserror
            .export _FindFile

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_FindFile:
        sta r6L
        stx r6H
        jsr FindFile
        jmp setoserror
