;
; Ullrich von Bassewitz, 14.09.2001
;

	.export		_clrscr
	.import		plot

	.include	"zeropage.inc"
	.include	"io.inc"

; ------------------------------------------------------------------------
; void __fastcall__ clrscr (void);

.proc	_clrscr

	lda	#0
	sta	CURS_X
	sta	CURS_Y
       	jsr	plot		; Set cursor to top left corner

	lda	#$20	   	; Screencode for blank
	ldx    	#$00
	ldy	#$00
	jsr	clearpage
	jsr	clearpage
	jsr	clearpage
	ldx	#<(40*25)
	jsr	clearpage	; Clear remainder of last page
	jmp	plot	   	; Set screen pointer again

.endproc


.proc	clearpage

@L1:	sta	(SCREEN_PTR),y
   	iny
	dex
   	bne	@L1
   	inc	SCREEN_PTR+1
	rts

.endproc
