;
; Ullrich von Bassewitz, 2011-07-10
;
; CC65 runtime: 8x16 => 24 unsigned multiplication
;

        .export         umul8x16r24, umul8x16r24m
        .export         umul8x16r16, umul8x16r16m

        .include        "zeropage.inc"

        .macpack        cpu

;---------------------------------------------------------------------------
; 8x16 => 24 unsigned multiplication routine. Because the overhead for a
; 8x16 => 16 unsigned multiplication routine is small, we will tag it with
; the matching labels, as well.
;
;  routine         LHS         RHS        result          result also in
; -----------------------------------------------------------------------
;  umul8x16r24     ax          ptr1-low   ax:sreg-low     ptr1:sreg-low
;  umul8x16r24m    ptr3        ptr1-low   ax:sreg-low     ptr1:sreg-low
;
; ptr3 is left intact by the routine.
;

umul8x16r24:
umul8x16r16:
        sta     ptr3
        stx     ptr3+1

umul8x16r24m:
umul8x16r16m:
.if (.cpu .bitand ::CPU_ISET_65SC02)
        stz     ptr1+1
        stz     sreg
.else
        ldx     #0
        stx     ptr1+1
        stx     sreg
.endif

        ldy     #8              ; Number of bits
        lda     ptr1
        ror     a               ; Get next bit into carry
@L0:    bcc     @L1

        clc
        tax
        lda     ptr3
        adc     ptr1+1
        sta     ptr1+1
        lda     ptr3+1
        adc     sreg
        sta     sreg
        txa

@L1:    ror     sreg
        ror     ptr1+1
        ror     a
        dey
        bne     @L0

        sta     ptr1            ; Save low byte of result
        ldx     ptr1+1          ; Load high byte of result
        rts                     ; Done


