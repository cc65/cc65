;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char GetDirHead (void);

            .import setoserror
            .export _GetDirHead

            .include "jumptab.inc"
            .include "diskdrv.inc"

_GetDirHead:
        jsr GetDirHead
        jmp setoserror
