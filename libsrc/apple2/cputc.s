;
; Ullrich von Bassewitz, 06.08.1998
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

   	.export		_cputcxy, _cputc
	.export		_gotoxy, cputdirect
	.export		newline, putchar

	.import		popa

	.include	"apple2.inc"

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

cputdirect:
	jsr	putchar
	;; Bump to next column
	inc	CH
	lda	CH
	cmp	MAX_X
	bne	return
	lda	#$00
	sta	CH
return:	
	rts

putchar:	
	ora	#$80		; Turn on high bit
	and	TEXTTYP		; Apply normal, inverse, flash
	ldy	CH
	ldx	RD80COL		; In 80 column mode?
	bpl	col40		; No, in 40 cols
	pha
	tya
	lsr			; Div by 2
	tay
	pla
	bcs	col40		; odd cols go in 40 col memory
	sta	PG2ON
col40:	sta	(BASL),Y
	sta	PG2OFF
	rts

newline:
	lda	CH
	pha
	inc	CV
	lda	CV
	cmp	MAX_Y
	bne	L2
	lda	#$00
	sta	CV
L2:
	jsr	VTABZ
	pla
	sta	CH
	rts
		
_gotoxy:
	sta	CV		; Store Y
	jsr	VTABZ
	jsr	popa		; Get X
	sta	CH		; Store X
	rts


