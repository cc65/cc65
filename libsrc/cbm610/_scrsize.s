;
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;

	.export	  	screensize

        .include        "cbm610.inc"

.proc   screensize

        ldx     #XSIZE
        ldy     #YSIZE
        rts

.endproc


