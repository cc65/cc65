;
; Maciej 'YTM/Elysium' Witkowiak
;
; 30.10.99, 17.8.2003

; void PutDecimal         (char style, unsigned value, char y, int x);

            .import popa, popax
            .import getintcharint
            .export _PutDecimal

            .include "jumptab.inc"

_PutDecimal:
        jsr getintcharint
        jsr popa
        jmp PutDecimal
