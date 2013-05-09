;
; Ullrich von Bassewitz, 2002-12-03
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using */
;

        .include        "get_tv.inc"
        .include        "c128.inc"


;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        ldx     #TV::PAL        ; Assume PAL
        lda     PALFLAG
        bne     pal
        dex                     ; NTSC
pal:    txa
        ldx     #0
        rts

.endproc
