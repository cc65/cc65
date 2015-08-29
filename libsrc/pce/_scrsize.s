;
; Screen size variables
;
        .include        "pce.inc"

        .export screensize
screensize:
        ldx     xsize
        ldy     ysize
        rts

; FIXME: changing the video mode allows for different screen sizes

.rodata
        .export xsize, ysize

xsize:  .byte   charsperline
ysize:  .byte   screenrows
