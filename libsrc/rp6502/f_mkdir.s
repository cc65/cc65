;
; int __fastcall__ f_mkdir (const char* name);
;

        .export         _f_mkdir

        .import         __ria_push_path

        .include        "rp6502.inc"

_f_mkdir:
        jsr     __ria_push_path
        bmi     @done
        lda     #RIA_OP_MKDIR
        sta     RIA_OP
        jmp     RIA_SPIN
@done:  rts
