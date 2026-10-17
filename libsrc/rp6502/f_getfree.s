;
; int __fastcall__ f_getfree (const char* name, unsigned long* free, unsigned long* total);
;

        .export         _f_getfree

        .import         __ria_push_path
        .import         popax

        .importzp       ptr2, ptr3

        .include        "rp6502.inc"

_f_getfree:
        sta     ptr2
        stx     ptr2+1          ; total
        jsr     popax
        sta     ptr3
        stx     ptr3+1          ; free
        jsr     popax
        jsr     __ria_push_path ; Y = 0
        bmi     @done
        lda     #RIA_OP_GETFREE
        sta     RIA_OP
        jsr     RIA_SPIN
        bmi     @done
@free:  lda     RIA_XSTACK
        sta     (ptr3),y
        iny
        cpy     #4
        bne     @free
        ldy     #0
@total: lda     RIA_XSTACK
        sta     (ptr2),y
        iny
        cpy     #4
        bne     @total
        txa                     ; success is 0
@done:  rts
