;
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;

	.export	      	screensize

        .include        "../apple2/apple2.inc"

screensize:
	ldx	WNDWDTH
	ldy	#24
        rts
