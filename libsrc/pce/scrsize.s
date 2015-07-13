;
; Screen size variables
;

                .export _screensize
_screensize:
                ldx     xsize
                ldy     ysize
                rts

; FIXME: changing the video mode allows for different screen sizes

.rodata
                .export xsize, ysize

xsize:          .byte 64
ysize:          .byte 28
