;
; Christian Groessler, 2014
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using */
;

        .include        "get_tv.inc"


;--------------------------------------------------------------------------
; _get_tv

.proc   _get_tv

        lda     #<TV::NTSC
        ldx     #>TV::NTSC
        rts

.endproc
