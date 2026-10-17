;
; long __fastcall__ f_telldir (int dirdes);
;

        .export         _f_telldir

        .importzp       sreg

        .include        "rp6502.inc"

_f_telldir:
        sta     RIA_A
        lda     #RIA_OP_TELLDIR
        sta     RIA_OP
        jsr     RIA_SPIN
        ldy     RIA_SREG
        sty     sreg
        ldy     RIA_SREG+1
        sty     sreg+1
        rts
