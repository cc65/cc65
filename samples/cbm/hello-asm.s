;
; Sample assembly program for Commodore machines
;

        .include "cbm_kernal.inc"

        ldx     #$00
:       lda     text,x
        beq     out
        jsr     CHROUT
        inx
        bne     :-
out:    rts

text:   .asciiz "hello world!"
