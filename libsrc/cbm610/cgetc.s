;
; Ullrich von Bassewitz, 06.08.1998
;
; char cgetc (void);
;

	.export	   	_cgetc
	.import	   	plot, write_crtc
	.import	   	cursor

	.include   	"cbm610.inc"


_cgetc:	lda    	KeyIndex	; Get number of characters
	bne	L2 	  	; Jump if there are already chars waiting

; Switch on the cursor if needed

       	lda	cursor
       	beq	L1 	    	; Jump if no cursor

       	jsr	plot		; Set the current cursor position
       	ldy	#10
       	lda	Config 	       	; Cursor format
       	jsr	write_crtc	; Set the cursor formar

L1:    	lda	KeyIndex
       	beq	L1

       	ldy	#10
       	lda	#$20		; Cursor off
       	jsr	write_crtc

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

