;
; Screen size variables
;
        .include        "gamate.inc"

        .export screensize
screensize:
        ldx     xsize
        ldy     ysize
        rts

.rodata
        .export xsize, ysize

xsize:  .byte   charsperline
ysize:  .byte   screenrows
