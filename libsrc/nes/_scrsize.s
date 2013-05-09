;
; Ullrich von Bassewitz, 2003-04-14
; Stefan Haubenthal, 2011-05-11
;
; Screen size function
;

        .export         screensize

        .include        "nes.inc"
        .include        "get_tv.inc"


.proc   screensize

        jsr     _get_tv
        ldx     #charsperline
        ldy     #screenrows
        cmp     #TV::PAL
        beq     pal
        dey             ; NTSC rows
pal:    rts

.endproc
