;
; Ullrich von Bassewitz, 06.08.1998
;
; char cgetc (void);
;

	.export	   	_cgetc
	.import	   	plot, write_crtc
	.import	   	cursor

	.include   	"zeropage.inc"
	.include	"page3.inc"


; ------------------------------------------------------------------------

.proc	_cgetc

L1:    	lda	KeyIndex
       	beq	L1

L2:    	ldx    	#$00		; Get index
       	ldy	KeyBuf		; Get first character in the buffer
       	sei
L3:    	lda	KeyBuf+1,x	; Move up the remaining chars
       	sta	KeyBuf,x
       	inx
       	cpx	KeyIndex
       	bne	L3
       	dec	KeyIndex
       	cli

       	ldx	#$00		; High byte
       	tya			; First char from buffer
       	rts

.endproc

