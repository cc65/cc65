;
; Ullrich von Bassewitz, 02.06.1998
;
; int isalnum (int c);
;

        .export         _isalnum
        .include        "ctype.inc"

_isalnum:
        cpx     #$00            ; Char range ok?
        bne     @L1             ; Jump if no
        tay
        lda     __ctype,y       ; Get character classification
        and     #CT_ALNUM       ; Mask character/digit bits
        rts

@L1:    lda     #$00            ; Return false
        tax
        rts

