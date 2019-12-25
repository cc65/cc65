;
; 2019-12-22, Greg King
;
; void __fastcall__ set_tv (unsigned char);
; /* Set the video mode the machine will use. */
;

        .export         _set_tv

        .include        "cx16.inc"


.proc   _set_tv
        ; Point to the video output register.

        stz     VERA::CTRL              ; Use port 0
        ldx     #<VERA::COMPOSER::VIDEO
        ldy     #>VERA::COMPOSER::VIDEO
        stx     VERA::ADDR
        sty     VERA::ADDR+1
        ldx     #^VERA::COMPOSER::VIDEO
        stx     VERA::ADDR+2

        sta     VERA::DATA0
        rts
.endproc
