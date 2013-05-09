;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char ReadByte  (void);

            .import __oserror
            .export _ReadByte

            .include "jumptab.inc"

_ReadByte:
        jsr ReadByte
        stx __oserror
        ldx #0
        rts
