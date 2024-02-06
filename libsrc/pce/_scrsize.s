;
; Screen size variables
;
        .export         screensize
        .export         xsize, ysize

        .include        "pce.inc"

screensize:
        ldx     xsize
        ldy     ysize
        rts

; FIXME: changing the video mode allows for different screen sizes

.rodata

xsize:  .byte   charsperline
ysize:  .byte   screenrows
