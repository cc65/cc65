;
; Ullrich von Bassewitz, 02.06.1998
;
; int isxdigit (int c);
;

        .export         _isxdigit
        .include        "ctype.inc"

_isxdigit:
        cpx     #$00            ; Char range ok?
        bne     @L1             ; Jump if no
        tay
        lda     __ctype,y       ; Get character classification
        and     #CT_XDIGIT      ; Mask xdigit bit
        rts

@L1:    lda     #$00            ; Return false
        tax
        rts

