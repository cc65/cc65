; Screen size function
;

    .export         screensize

		
.proc   screensize		
        ldx     #80
        ldy     #50
		rts
.endproc