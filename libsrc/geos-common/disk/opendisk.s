;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char OpenDisk (void);

            .import setoserror
            .export _OpenDisk

            .include "jumptab.inc"
            .include "diskdrv.inc"
        
_OpenDisk:
        jsr OpenDisk
        jmp setoserror
