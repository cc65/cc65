;
; Ullrich von Bassewitz, 02.06.1998
;
; int isalpha (int c);
;

        .export         _isalpha
        .include        "ctype.inc"

_isalpha:
        cpx     #$00            ; Char range ok?
        bne     @L1             ; Jump if no
        tay
        lda     __ctype,y       ; Get character classification
        and     #CT_ALPHA       ; Mask character bits
        rts

@L1:    lda     #$00            ; Return false
        tax
        rts

