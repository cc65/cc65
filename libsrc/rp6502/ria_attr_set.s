;
; int __fastcall__ ria_attr_set (long val, unsigned char id);
;

        .export         _ria_attr_set

        .import         popeax

        .importzp       sreg

        .include        "rp6502.inc"

_ria_attr_set:
        sta     RIA_A
        jsr     popeax
        ldy     sreg+1
        sty     RIA_XSTACK
        ldy     sreg
        sty     RIA_XSTACK
        stx     RIA_XSTACK
        sta     RIA_XSTACK
        lda     #RIA_OP_ATTR_SET
        sta     RIA_OP
        jmp     RIA_SPIN
