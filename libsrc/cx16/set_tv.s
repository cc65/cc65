;
; 2019-12-22, Greg King
;
; void __fastcall__ set_tv (unsigned char);
; /* Set the video mode the machine will use. */
;

        .export         _set_tv

        .importzp       tmp1
        .include        "cx16.inc"


.proc   _set_tv
        ; Point to the video output register.

        ; Only set bits for the Output field (0-1)
        and     #$03
        sta     tmp1

        ; Clear out bits 0-1 from current DC_VIDEO
        lda     VERA::DC_VIDEO
        and     #$FC

        ; Set bits 0-1 from tmp1
        ora     tmp1

        ; Update DC_VIDEO
        sta     VERA::DC_VIDEO
        rts
.endproc
