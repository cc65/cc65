;
; 2019-09-20, Greg King
;
; void __fastcall__ set_tv (unsigned char);
; /* Set the video mode the machine will use. */
;

        .export         _set_tv

        .include        "cx16.inc"


.proc   _set_tv
        php
        pha
        sei                             ; Don't let interrupts interfere

        ; Point to the video output register.

        stz     VERA::CTRL              ; Use port 0
        lda     #<VERA::COMPOSER::VIDEO
        ldx     #>VERA::COMPOSER::VIDEO
        ldy     #^VERA::COMPOSER::VIDEO
        sta     VERA::ADDR
        stx     VERA::ADDR+1
        sty     VERA::ADDR+2

        pla
        sta     VERA::DATA0
        plp                             ; Re-enable interrupts
        rts
.endproc
