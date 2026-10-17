;
; void __randomize (void);
;

        .export         ___randomize

        .import         _srand

        .include        "rp6502.inc"

___randomize:
        lda     #RIA_ATTR_LRAND
        sta     RIA_A
        lda     #RIA_OP_ATTR_GET
        sta     RIA_OP
        jsr     RIA_SPIN
        jmp     _srand
