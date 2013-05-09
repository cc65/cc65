;
; Ullrich von Bassewitz, 02.06.1998
;
; int isprint (int c);
;

        .export         _isprint
        .include        "ctype.inc"

_isprint:
        cpx     #$00            ; Char range ok?
        bne     @L1             ; Jump if no
        tay
        lda     __ctype,y       ; Get character classification
        eor     #CT_CTRL        ; NOT a control char
        and     #CT_CTRL        ; Mask control char bit
        rts

@L1:    lda     #$00            ; Return false
        tax
        rts

