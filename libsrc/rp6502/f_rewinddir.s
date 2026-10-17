;
; int __fastcall__ f_rewinddir (int dirdes);
;

        .export         _f_rewinddir

        .include        "rp6502.inc"

_f_rewinddir:
        sta     RIA_A
        lda     #RIA_OP_REWINDDIR
        sta     RIA_OP
        jmp     RIA_SPIN
