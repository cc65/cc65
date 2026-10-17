;
; int __fastcall__ chdir (const char* name);
;

        .export         _chdir

        .import         __ria_push_path

        .include        "rp6502.inc"

_chdir:
        jsr     __ria_push_path
        bmi     @done
        lda     #RIA_OP_CHDIR
        sta     RIA_OP
        jmp     RIA_SPIN
@done:  rts
