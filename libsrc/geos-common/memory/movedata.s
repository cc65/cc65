;
; Maciej 'YTM/Elysium' Witkowiak
;
; 30.10.99, 15.07.2001

; void* MoveData         (char* dest, char *source, int length);

            .import popax
            .export _MoveData

            .include "jumptab.inc"
            .include "geossym.inc"

_MoveData:
        sta r2L
        stx r2H
        jsr popax
        sta r0L
        stx r0H
        jsr popax
        sta r1L
        stx r1H
        jsr MoveData
        lda r1L                 ; return dest ptr to be compatible with memmove and memcpy
        ldx r1H
        rts
