;
; int __fastcall__ ria_rln_peek (char* peek, unsigned char* pos);
;

        .export         _ria_rln_peek

        .import         popptr1

        .importzp       ptr1, ptr2, tmp1

        .include        "rp6502.inc"

_ria_rln_peek:
        sta     ptr2
        stx     ptr2+1          ; pos
        jsr     popptr1         ; peek, Y = 0
        lda     #RIA_OP_RLN_PEEK
        sta     RIA_OP
        jsr     RIA_SPIN
        bmi     @done
        sta     tmp1
        lda     RIA_XSTACK
        sta     (ptr2)
@copy:  lda     RIA_XSTACK
        sta     (ptr1),y
        cpy     tmp1
        iny
        bcc     @copy
        lda     tmp1
@done:  rts
