;
; int __fastcall__ remove (const char* name);
;

        .export         _remove

        .import         __ria_push_path

        .include        "rp6502.inc"

_remove:
        jsr     __ria_push_path
        bmi     @done
        lda     #RIA_OP_UNLINK
        sta     RIA_OP
        jmp     RIA_SPIN
@done:  rts
