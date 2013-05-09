;
; Maciej 'YTM/Elysium' Witkowiak
;
; 22.12.99, 29.07.2000

; void CopyFString (char length, char *dest, char* source);

            .import DoubleSPop, SetPtrXY
            .import popa
            .export _CopyFString

            .include "jumptab.inc"

_CopyFString:
        jsr DoubleSPop
        jsr popa
        jsr SetPtrXY
        jmp CopyFString
