;
; Ullrich von Bassewitz, 2010-11-03
;
; CC65 runtime: 16x16 => 32 signed multiplication
;

        .export         imul16x16r32
        .import         negax, umul16x16r32m, negeax

        .include        "zeropage.inc"


;---------------------------------------------------------------------------
; 16x16 => 32 signed multiplication routine.
;
;   LHS         RHS           result          result also in
; -------------------------------------------------------------
;   ax          ptr1          ax:sreg
;
; There is probably a faster way to do this.
;

imul16x16r32:
        stx     tmp1
        cpx     #0
        bpl     @L1
        jsr     negax
@L1:    sta     ptr3
        stx     ptr3+1

        lda     ptr1+1
        eor     tmp1
        sta     tmp1
        lda     ptr1
        ldx     ptr1+1
        bpl     @L2
        jsr     negax
        sta     ptr1
        stx     ptr1+1
@L2:    jsr     umul16x16r32m
        ldy     tmp1
        bpl     @L3
        jmp     negeax
@L3:    rts

