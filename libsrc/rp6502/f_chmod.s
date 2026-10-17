;
; int __fastcall__ f_chmod (const char* path, unsigned char attr, unsigned char mask);
;

        .export         _f_chmod

        .import         __ria_push_path
        .import         popa, popax

        .importzp       tmp1

        .include        "rp6502.inc"

_f_chmod:
        sta     RIA_A           ; mask
        jsr     popa
        sta     tmp1            ; attr
        jsr     popax
        jsr     __ria_push_path
        bmi     @done
        lda     tmp1
        sta     RIA_XSTACK
        lda     #RIA_OP_CHMOD
        sta     RIA_OP
        jmp     RIA_SPIN
@done:  rts
