;
; Ullrich von Bassewitz, 13.09.2001
;
; udtim routine for the 510. We will not check for the stop key here, since
; C programs will not use it.
;

      	.export	    	k_udtim
	.importzp	time


.proc	k_udtim

	inc	time
	bne	L9
	inc	time+1
	bne	L9
	inc	time+2
	bne	L9
	inc	time+3
L9:	rts

.endproc


