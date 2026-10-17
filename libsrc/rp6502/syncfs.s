;
; int __fastcall__ syncfs (int fd);
;

        .export         _syncfs

        .include        "rp6502.inc"

_syncfs:
        sta     RIA_A
        lda     #RIA_OP_SYNCFS
        sta     RIA_OP
        jmp     RIA_SPIN
