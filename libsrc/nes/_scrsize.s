;
; Ullrich von Bassewitz, 2003-04-14
;
; Screen size function
;

	.export	   	screensize

        .include        "nes.inc"


.proc   screensize

        ldx     #charsperline
        ldy     #screenrows
        rts

.endproc


