;
; Ullrich von Bassewitz, 02.06.1998
;
; int isdigit (int c);
;

        .export         _isdigit
        .include        "ctype.inc"

_isdigit:
        cpx     #$00            ; Char range ok?
        bne     @L1             ; Jump if no
        tay
        lda     __ctype,y       ; Get character classification
        and     #CT_DIGIT       ; Mask digit bit
        rts

@L1:    lda     #$00            ; Return false
        tax
        rts

