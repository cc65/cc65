;
; Ullrich von Bassewitz, 06.08.1998
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

	.constructor	initconio
   	.export		_cputcxy, _cputc
	.export		_gotoxy, cputdirect
	.export		newline, putchar

	.import		popa, SETWND, BASCALC

	.include	"apple2.inc"

initconio:
	lda	#$FF		; Normal character display mode
	sta	INVFLG
	lda	#$00
	jsr	SETWND		; Reset text window to full screen
	rts

; Plot a character - also used as internal function

_cputcxy:
	pha			; Save C
	jsr	popa		; Get Y
	jsr	_gotoxy
	pla			; Restore C

_cputc:
	cmp	#$0D		; Test for \r = carrage return
	bne	L1
	lda	#$00		; Goto left edge of screen
	sta	CH
	rts			; That's all we do
L1:
	cmp	#$0A		; Test for \n = line feed
	beq	newline
	ora	#$80		; Turn on high bit
	cmp	#$E0		; Test for lowercase
	bmi	cputdirect
	and	#$DF		; Convert to uppercase

cputdirect:
	jsr	putchar
	inc	CH		; Bump to next column
	lda	CH
	cmp	#40
	bne	return
	lda	#$00
	sta	CH
return:	
	rts

putchar:	
	and	INVFLG		; Apply normal, inverse, flash
	ldy	CH
	sta	(BASL),Y
	rts

newline:
	lda	CH
	pha
	inc	CV
	lda	CV
	cmp	#24
	bne	L2
	lda	#$00
	sta	CV
L2:
	jsr	BASCALC
	pla
	sta	CH
	rts
		
_gotoxy:
	sta	CV		; Store Y
	jsr	BASCALC
	jsr	popa		; Get X
	sta	CH		; Store X
	rts
