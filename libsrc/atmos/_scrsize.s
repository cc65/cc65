;
; Ullrich von Bassewitz, 2003-04-13
;
; Screen size variables
;

	.export	   	screensize

.proc   screensize

        ldx     #40
        ldy     #28
        rts

.endproc


