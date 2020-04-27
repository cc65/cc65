;
; 2019-12-22, Greg King
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using. */
;

        .export         _get_tv

        .include        "cx16.inc"


.proc   _get_tv
        ; Get the current setting from the DC_VIDEO register
        
        lda     VERA::DC_VIDEO
        and     #$07                    ; Get the type of output signal
        rts

.endproc
