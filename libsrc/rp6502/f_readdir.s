;
; int __fastcall__ f_readdir (f_stat_t* dirent, int dirdes);
;

        .export         _f_readdir

        .import         popptr1

        .importzp       ptr1

        .include        "rp6502.inc"

_f_readdir:
        sta     RIA_A
        jsr     popptr1         ; dirent, Y = 0
        lda     #RIA_OP_READDIR
        sta     RIA_OP
        jsr     RIA_SPIN
        bmi     @done
@page:  lda     RIA_XSTACK
        sta     (ptr1),y
        iny
        bne     @page
        inc     ptr1+1
@rest:  lda     RIA_XSTACK
        sta     (ptr1),y
        iny
        cpy     #282 - 256      ; sizeof (f_stat_t)
        bne     @rest
        txa                     ; success is 0
@done:  rts
