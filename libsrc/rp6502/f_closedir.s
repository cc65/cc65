;
; int __fastcall__ f_closedir (int dirdes);
;

        .export         _f_closedir

        .include        "rp6502.inc"

_f_closedir:
        sta     RIA_A
        lda     #RIA_OP_CLOSEDIR
        sta     RIA_OP
        jmp     RIA_SPIN
