;
; Ullrich von Bassewitz, 07.08.1998
; Christian Krueger, 11-Mar-2017, added 65SC02 optimization
; CC65 runtime: modulo operation for long signed ints
;

; When negating values, we will ignore the possibility here, that one of the
; values if $8000, in which case the negate will fail.

        .export         tosmod0ax, tosmodeax
        .import         poplsargs, udiv32, negeax
        .importzp       sreg, ptr1, ptr2, tmp1, tmp3, tmp4

        .macpack        cpu

tosmod0ax:
.if (.cpu .bitand ::CPU_ISET_65SC02)
        stz     sreg
        stz     sreg+1
.else
        ldy     #$00
        sty     sreg
        sty     sreg+1
.endif

tosmodeax:                         
        jsr     poplsargs       ; Get arguments from stack, adjust sign
        jsr     udiv32          ; Do the division, remainder is in (ptr2:tmp3:tmp4)

; Load the result

        lda     ptr2
        ldx     ptr2+1
        ldy     tmp3
        sty     sreg
        ldy     tmp4
        sty     sreg+1

; Check the sign of the result. It is the sign of the left operand.

        bit     tmp1            ; Check sign of left operand
        bpl     Pos             ; Jump if result is positive

; Result is negative

        jmp     negeax          ; Negate result

; Result is positive

Pos:    rts                     ; Done

