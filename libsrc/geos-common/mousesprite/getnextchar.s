;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char GetNextChar (void);
;       note that if it returns 0 (FALSE) then no characters are available

            .export _GetNextChar

            .include "jumptab.inc"

_GetNextChar:
        jsr GetNextChar
        ldx #0
        tay                     ; preserve Z flag
        rts
