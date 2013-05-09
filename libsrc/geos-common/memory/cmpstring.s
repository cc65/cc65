;
; Maciej 'YTM/Elysium' Witkowiak
;
; 22.12.1999, 2.1.2003

; char CmpString (char *dest, char* source);

            .import DoubleSPop
            .import return0, return1
            .export _CmpString

            .include "jumptab.inc"

_CmpString:
        jsr DoubleSPop
        jsr CmpString
        bne L1
        jmp return0
L1:     jmp return1
