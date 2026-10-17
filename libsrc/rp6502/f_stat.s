;
; int __fastcall__ f_stat (const char* path, f_stat_t* dirent);
;

        .export         _f_stat

        .import         __ria_push_path
        .import         popax

        .importzp       ptr2

        .include        "rp6502.inc"

_f_stat:
        sta     ptr2
        stx     ptr2+1
        jsr     popax
        jsr     __ria_push_path ; Y = 0
        bmi     @done
        lda     #RIA_OP_STAT
        sta     RIA_OP
        jsr     RIA_SPIN
        bmi     @done
@page:  lda     RIA_XSTACK
        sta     (ptr2),y
        iny
        bne     @page
        inc     ptr2+1
@rest:  lda     RIA_XSTACK
        sta     (ptr2),y
        iny
        cpy     #282 - 256      ; sizeof (f_stat_t)
        bne     @rest
        txa                     ; success is 0
@done:  rts
