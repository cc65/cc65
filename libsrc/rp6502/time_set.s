;
; int __fastcall__ time_set (unsigned long time);
;

        .export         _time_set

        .importzp       sreg

        .include        "rp6502.inc"

_time_set:
        stz     RIA_XSTACK      ; zero on top keeps the unsigned time positive
        ldy     sreg+1
        sty     RIA_XSTACK
        ldy     sreg
        sty     RIA_XSTACK
        stx     RIA_XSTACK
        sta     RIA_XSTACK
        lda     #RIA_OP_TIME_SET
        sta     RIA_OP
        jmp     RIA_SPIN
