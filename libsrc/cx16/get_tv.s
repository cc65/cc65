;
; 2019-12-22, Greg King
;
; unsigned char get_tv (void);
; /* Return the video mode the machine is using. */
;

        .export         _get_tv

        .include        "cx16.inc"


.proc   _get_tv
        ; Point to the video output register.

        stz     VERA::CTRL              ; Use port 0
        lda     #<VERA::COMPOSER::VIDEO
        ldx     #>VERA::COMPOSER::VIDEO
        ldy     #^VERA::COMPOSER::VIDEO
        sta     VERA::ADDR
        stx     VERA::ADDR+1
        sty     VERA::ADDR+2

        lda     VERA::DATA0
        and     #$07                    ; Get the type of output signal
        rts
.endproc
