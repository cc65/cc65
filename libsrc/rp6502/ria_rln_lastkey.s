;
; int __fastcall__ ria_rln_lastkey (char* key, unsigned char* action);
;

        .export         _ria_rln_lastkey

        .import         popptr1

        .importzp       ptr1, ptr2, tmp1

        .include        "rp6502.inc"

_ria_rln_lastkey:
        sta     ptr2
        stx     ptr2+1          ; action
        jsr     popptr1         ; key, Y = 0
        lda     #RIA_OP_RLN_LASTKEY
        sta     RIA_OP
        jsr     RIA_SPIN
        bmi     @done
        sta     tmp1
        cmp     #1
        bcc     @done           ; no key
        lda     RIA_XSTACK
        sta     (ptr2)
@copy:  lda     RIA_XSTACK
        sta     (ptr1),y
        iny
        cpy     tmp1
        bne     @copy
        lda     tmp1
@done:  rts
