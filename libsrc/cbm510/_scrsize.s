;
; Ullrich von Bassewitz, 13.09.2000
;
; Screen size variables
;

	.export		screensize

        .include        "cbm510.inc"

.proc   screensize

        ldx     #XSIZE
        ldy     #YSIZE
        rts

.endproc


