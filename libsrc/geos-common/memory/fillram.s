;
; Maciej 'YTM/Elysium' Witkowiak
;
; 30.10.99, 15.07.2001

; void * FillRam         (char *dest, char what, int length);

            .import popa, ___memset_common
            .export _FillRam

            .include "geossym.inc"

_FillRam:
        sta r0L
        stx r0H
        jsr popa                ; pop 'char what'
        jmp ___memset_common
