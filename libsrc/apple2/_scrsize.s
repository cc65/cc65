;
; Ullrich von Bassewitz, 26.10.2000
;
; Screen size variables
;

	.export	      	screensize

        .include        "apple2.inc"

.proc   screensize

        ldx     MAX_X
        ldy     MAX_Y
        rts

.endproc

