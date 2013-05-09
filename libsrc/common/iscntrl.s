;
; Ullrich von Bassewitz, 02.06.1998
;
; int iscntrl (int c);
;

        .export         _iscntrl
        .include        "ctype.inc"

_iscntrl:
        cpx     #$00            ; Char range ok?
        bne     @L1             ; Jump if no
        tay
        lda     __ctype,y       ; Get character classification
        and     #CT_CTRL        ; Mask control character bit
        rts

@L1:    lda     #$00            ; Return false
        tax
        rts

