;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void PutString        (char *mytxt, char y, int x);

            .import popax, popa
            .import getintcharint
            .export _PutString

            .include "jumptab.inc"

_PutString:
        jsr getintcharint
        jmp PutString
