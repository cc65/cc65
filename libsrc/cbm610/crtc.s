;
; Ullrich von Bassewitz, 28.09.1998
;
; Write to the CRTC.
;

 	.export	       	write_crtc, read_crtc
	.importzp	crtc

	.include      	"cbm610.inc"


; Write a value to the CRTC. The index is in Y, the value in A

.proc	write_crtc
	sta	sedt1
	lda	IndReg
	pha
	lda	#$0F
	sta	IndReg
	tya
	ldy	#$00
	sei
	sta	(crtc),y
	iny
	lda	sedt1
	sta	(crtc),y
	cli
	pla
	sta	IndReg
	lda	sedt1
	rts
.endproc


.proc	read_crtc
	sty    	sedt1
	lda	IndReg
	pha
	lda	#$0F
	sta	IndReg
	lda    	sedt1
	ldy	#$00
	sei
	sta	(crtc),y
	iny
       	lda	(crtc),y
	cli
	tay
	pla
	sta	IndReg
	tya
	ldy    	sedt1
	rts
.endproc


