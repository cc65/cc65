;
; Ullrich von Bassewitz, 02.06.1998
;
; int islower (int c);
;

        .export         _islower
        .include        "ctype.inc"

_islower:
        cpx     #$00            ; Char range ok?
        bne     @L1             ; Jump if no
        tay
        lda     __ctype,y       ; Get character classification
        and     #CT_LOWER       ; Mask lower char bit
        rts

@L1:    lda     #$00            ; Return false
        tax
        rts

