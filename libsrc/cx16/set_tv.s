;
; 2020-05-02, Greg King
;
; void __fastcall__ set_tv (unsigned char);
; /* Set the video mode the machine will use. */
;

        .export         _set_tv

        .include        "cx16.inc"


.proc   _set_tv
        stz     VERA::CTRL              ; Use display register bank 0
        eor     VERA::DISP::VIDEO
        and     #%00000111
        eor     VERA::DISP::VIDEO       ; Replace old mode with new mode
        sta     VERA::DISP::VIDEO
        rts
.endproc
