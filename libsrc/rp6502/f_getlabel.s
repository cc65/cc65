;
; int __fastcall__ f_getlabel (const char* path, char* label);
;

        .export         _f_getlabel

        .import         __ria_push_path
        .import         popax

        .importzp       ptr2

        .include        "rp6502.inc"

_f_getlabel:
        sta     ptr2
        stx     ptr2+1
        jsr     popax
        jsr     __ria_push_path ; Y = 0
        bmi     @done
        lda     #RIA_OP_GETLABEL
        sta     RIA_OP
        jsr     RIA_SPIN        ; length including the terminator
        bmi     @done
        pha
        dey
@copy:  iny
        lda     RIA_XSTACK
        sta     (ptr2),y
        bne     @copy
        pla
@done:  rts
