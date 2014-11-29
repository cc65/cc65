;
; Screen size variables
;



.export _screensize
_screensize:
        ldx     xsize
        ldy     ysize
        rts

.rodata
	.export		xsize, ysize

xsize: 	.byte 64
ysize:	.byte 28
