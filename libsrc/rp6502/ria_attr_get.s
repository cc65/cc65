;
; long __fastcall__ ria_attr_get (unsigned char id);
;

        .export         _ria_attr_get

        .importzp       sreg

        .include        "rp6502.inc"

_ria_attr_get:
        sta     RIA_A
        lda     #RIA_OP_ATTR_GET
        sta     RIA_OP
        jsr     RIA_SPIN
        ldy     RIA_SREG
        sty     sreg
        ldy     RIA_SREG+1
        sty     sreg+1
        rts
