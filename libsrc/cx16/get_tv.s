;
; 2020-05-02, Greg King
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using. */
;

        .export         _get_tv

        .include        "cx16.inc"


.proc   _get_tv
        stz     VERA::CTRL              ; Use display register bank 0
        lda     VERA::DISP::VIDEO
        and     #%00000111              ; Get the type of output signal
        ldx     #>$0000                 ; Promote to unsigned int
        rts
.endproc
