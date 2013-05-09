;
; Ullrich von Bassewitz, 02.06.1998
;
; int isblank (int c);
;
; cc65 (and GNU) extension.
;

        .export         _isblank
        .include        "ctype.inc"

_isblank:
        cpx     #$00            ; Char range ok?
        bne     @L1             ; Jump if no
        tay
        lda     __ctype,y       ; Get character classification
        and     #CT_SPACE_TAB   ; Mask blank bit
        rts

@L1:    lda     #$00            ; Return false
        tax
        rts

