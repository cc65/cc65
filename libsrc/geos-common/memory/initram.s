;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; void InitRam         (struct inittab*);

            .export _InitRam

            .include "jumptab.inc"
            .include "geossym.inc"

_InitRam:
        sta r0L
        stx r0H
        jmp InitRam
