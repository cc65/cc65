;
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;

        .export         screensize

        .include        "agat.inc"

screensize:
        ldx     WNDWDTH
        lda     WNDBTM
        sec
        sbc     WNDTOP
        tay
        rts
