;
; Stefan Haubenthal, 2009-08-02
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using */
;

        .include        "get_tv.inc"


;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        lda     #<TV::OTHER     ; CRTC
        ldx     #>TV::OTHER
        rts

.endproc
