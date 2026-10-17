;
; int __fastcall__ f_seekdir (long offs, int dirdes);
;

        .export         _f_seekdir

        .import         popeax

        .importzp       sreg

        .include        "rp6502.inc"

_f_seekdir:
        sta     RIA_A
        jsr     popeax
        ldy     sreg+1
        sty     RIA_XSTACK
        ldy     sreg
        sty     RIA_XSTACK
        stx     RIA_XSTACK
        sta     RIA_XSTACK
        lda     #RIA_OP_SEEKDIR
        sta     RIA_OP
        jmp     RIA_SPIN
