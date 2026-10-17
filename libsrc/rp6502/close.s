;
; int __fastcall__ close (int fd);
;

        .export         _close

        .include        "rp6502.inc"

_close:
        sta     RIA_A
        lda     #RIA_OP_CLOSE
        sta     RIA_OP
        jmp     RIA_SPIN
