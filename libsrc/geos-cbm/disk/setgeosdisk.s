;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char SetGEOSDisk (void);

            .export _SetGEOSDisk
            .import setoserror

            .include "jumptab.inc"

_SetGEOSDisk:
        jsr SetGEOSDisk
        jmp setoserror
