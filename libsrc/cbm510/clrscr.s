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

	ldx	#4
	ldy	#$00
	lda	#$20	   	; Screencode for blank
L1:	sta	(CharPtr),y
   	iny
   	bne	L1
   	inc	CharPtr+1
	dex
	bne	L1

	jmp	plot		; Set screen pointer again

.endproc



