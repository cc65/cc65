;
; Ullrich von Bassewitz, 2010-11-03
;
; CC65 runtime: 16x16 => 32 unsigned multiplication
;

        .export         umul16x16r32, umul16x16r32m
        .export         umul16x16r16, umul16x16r16m

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 16x16 => 32 unsigned multiplication routine. Because the overhead for a
; 16x16 => 16 unsigned multiplication routine is small, we will tag it with 
; the matching labels, as well.
;
;  routine         LHS         RHS        result          result also in
; -----------------------------------------------------------------------
;  umul16x16r32    ax          ptr1       ax:sreg          ptr1:sreg
;  umul16x16r32m   ptr3        ptr1       ax:sreg          ptr1:sreg
;  umul16x16r16    ax          ptr1       ax               ptr1
;  umul16x16r16m   ptr3        ptr1       ax               ptr1
;
; ptr3 is left intact by the routine.
;

umul16x16r32:
umul16x16r16:
        sta     ptr3
        stx     ptr3+1

umul16x16r32m:
umul16x16r16m:
        lda     #0
        sta     sreg+1
        ldy     #16             ; Number of bits

        lsr     ptr1+1
        ror     ptr1            ; Get first bit into carry
@L0:    bcc     @L1

        clc
        adc     ptr3
        tax
        lda     ptr3+1
        adc     sreg+1
        sta     sreg+1
        txa

@L1:    ror     sreg+1
        ror     a
        ror     ptr1+1
        ror     ptr1
        dey
        bne     @L0

        sta     sreg            ; Save byte 3
        lda     ptr1            ; Load the result
        ldx     ptr1+1
        rts                     ; Done


