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

; ------------------------------------------------------------------------
; Initialization

.segment        "INIT"

initconio:
	lda	#$FF		; Normal character display mode
	sta	INVFLG
	lda	#$00
       	jmp	SETWND		; Reset text window to full screen

; ------------------------------------------------------------------------
; Plot a character - also used as internal function

.code

_cputcxy:
     	pha			; Save C
     	jsr	popa		; Get Y
     	jsr	_gotoxy
     	pla			; Restore C

_cputc:
     	cmp	#$0D		; Test for \r = carrage return
     	beq	left
     	cmp	#$0A		; Test for \n = line feed
     	beq	newline
     	ora	#$80		; Turn on high bit
     	cmp	#$E0		; Test for lowercase
     	bcc	cputdirect
     	and	#$DF		; Convert to uppercase

cputdirect:
     	jsr	putchar
     	inc	CH		; Bump to next column
     	lda	CH
     	cmp	#40
     	bne	done
left:	lda	#$00		; Goto left edge of screen
     	sta	CH
done:	rts

newline:
     	inc	CV
     	lda	CV
     	cmp	#24
     	bne	:+
     	lda	#$00
     	sta	CV
:      	jmp     BASCALC

putchar:
     	and	INVFLG		; Apply normal, inverse, flash
     	ldy	CH
     	sta	(BASL),Y
     	rts

_gotoxy:
     	sta	CV		; Store Y
     	jsr	BASCALC
     	jsr	popa		; Get X
     	sta	CH		; Store X
     	rts
