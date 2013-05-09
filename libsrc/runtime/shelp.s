;
; Ullrich von Bassewitz, 07.08.1998
;
; CC65 runtime: helper stuff for mod/div/mul with signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $8000, in which case the negate will fail.

        .export         popsargs
        .import         negax, popax
        .importzp       sreg, tmp1, tmp2, ptr4

popsargs:
        stx     tmp2            ; Remember sign
        cpx     #0
        bpl     L1
        jsr     negax           ; Negate accumulator
L1:     sta     ptr4
        stx     ptr4+1          ; Save right operand

        jsr     popax
        stx     tmp1            ; Remember sign
        cpx     #0
        bpl     L2
        jsr     negax
L2:     sta     sreg
        stx     sreg+1
        rts

