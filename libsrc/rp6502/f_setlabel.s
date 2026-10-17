;
; int __fastcall__ f_setlabel (const char* name);
;

        .export         _f_setlabel

        .import         __ria_push_path

        .include        "rp6502.inc"

_f_setlabel:
        jsr     __ria_push_path
        bmi     @done
        lda     #RIA_OP_SETLABEL
        sta     RIA_OP
        jmp     RIA_SPIN
@done:  rts
