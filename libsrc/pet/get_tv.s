;
; Ullrich von Bassewitz, 2004-10-26
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using */
;

        .include        "pet.inc"
        .include        "get_tv.inc"

;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        lda     #TV::OTHER
        ldx     #0
        rts

.endproc
