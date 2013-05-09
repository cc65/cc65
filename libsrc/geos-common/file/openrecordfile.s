;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char OpenRecordFile  (char *myName);

            .import setoserror
            .export _OpenRecordFile

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_OpenRecordFile:
        sta r0L
        stx r0H
        jsr OpenRecordFile
        jmp setoserror
