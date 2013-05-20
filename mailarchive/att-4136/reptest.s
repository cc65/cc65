NREP = 10

.PROC xyz

.repeat NREP*8,xvar
	lda #xvar
.endrep
.ENDPROC

