;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char GetFHdrInfo  (struct filehandle *myFile);

            .import setoserror
            .export _GetFHdrInfo

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_GetFHdrInfo:
        sta r9L
        stx r9H
        jsr GetFHdrInfo
        jmp setoserror
