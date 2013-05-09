;
; Ullrich von Bassewitz, 02.06.1998
;
; int isspace (int c);
;

        .export         _isspace
        .include        "ctype.inc"

_isspace:
        cpx     #$00            ; Char range ok?
        bne     @L1             ; Jump if no
        tay
        lda     __ctype,y       ; Get character classification
        and     #(CT_SPACE | CT_OTHER_WS)   ; Mask space bits
        rts

@L1:    lda     #$00            ; Return false
        tax
        rts

