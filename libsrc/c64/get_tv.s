;
; Ullrich von Bassewitz, 2002-12-03
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using */
;

        .include        "c64.inc"
        .include        "get_tv.inc"

;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        lda     PALFLAG
        ldx     #0
        rts

.endproc
