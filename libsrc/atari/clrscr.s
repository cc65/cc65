;
; Christian Groessler, Apr-2000
;
; void clrscr (void);
;

	.export		_clrscr
	.include	"atari.inc"
	.importzp	ptr1
	.import		setcursor

_clrscr:lda	SAVMSC		; screen memory
	sta	ptr1
	lda	SAVMSC+1
	clc
	adc	#>(40*24)
	sta	ptr1+1
	lda	#0		; screen code of space char
	sta	OLDCHR
	ldy	#<(40*24)	; 40x24: size of default atari screen
	ldx	#>(40*24)
_clr1:	sta	(ptr1),y
	dey
	bne	_clr1
	sta	(ptr1),y
	dex
	bmi	done
	ldy	ptr1+1
	dey
	sty	ptr1+1
	ldy	#255
	jmp	_clr1

done:	sta	COLCRS
	sta	ROWCRS
	jmp	setcursor
