;
; Maciej 'YTM/Alliance' Witkowiak
;
; 22.12.99

; void CopyString (char *dest, char* source);

            .import DoubleSPop
            .export _CopyString

            .include "jumptab.inc"

_CopyString:
        jsr DoubleSPop
        jmp CopyString
