;
; Ullrich von Bassewitz, 13.09.2001
;
; PLOT routine for the 510.
;

      	.export		k_plot
	.importzp	crtc

      	.include      	"zeropage.inc"
	.include	"io.inc"
	
	
; ------------------------------------------------------------------------
;

.proc	k_plot

	bcc    	set
	ldx    	CURS_Y
	ldy    	CURS_X
	rts

set:   	stx    	CURS_Y
     	sty    	CURS_X

	lda    	LineLSBTab,x
	sta    	CharPtr
	lda    	LineMSBTab,x
	sta    	CharPtr+1

.if	0
	lda    	IndReg
	pha
	lda    	#$0F
	sta    	IndReg

	ldy    	#$00
	clc
	sei
	sta    	(crtc),y
	lda    	CharPtr
	adc    	CURS_X
	iny
	sta    	(crtc),y
	dey
	lda    	#$0E
	sta    	(crtc),y
	iny
	lda    	(crtc),y
	and    	#$F8
	sta    	sedt1
	lda    	CharPtr+1
	adc    	#$00
	and    	#$07
	ora    	sedt1
	sta    	(crtc),y
	cli

	pla
	sta    	IndReg
.endif
	rts
.endproc

; -------------------------------------------------------------------------
; Low bytes of the start address of the screen lines

.rodata

.macro  LineLoTab
	.repeat 25, I
	.byte   <(VIDEO_RAM + I * 40)
	.endrep
.endmacro

LineLSBTab:    	LineLoTab

; -------------------------------------------------------------------------
; High bytes of the start address of the screen lines

.macro  LineHiTab
	.repeat 25, I
	.byte   >(VIDEO_RAM + I * 40)
	.endrep
.endmacro

LineMSBTab:	LineHiTab


