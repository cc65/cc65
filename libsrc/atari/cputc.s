;
; Mark Keates, Christian Groessler
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

    	.export	       	_cputcxy, _cputc
	.export		plot, cputdirect, putchar
	.import		popa, _gotoxy, mul40
	.importzp	tmp4,ptr4
	.import		_revflag

	.include	"atari.inc"

_cputcxy:
	pha	    		; Save C
	jsr	popa		; Get Y
	jsr	_gotoxy		; Set cursor, drop x
	pla			; Restore C

_cputc:
	cmp	#$0D		; CR
	bne	L4
	lda	#0
	sta	COLCRS
	beq	plot		; return
	
L4:	cmp	#$0A		; LF
	beq	newline
	cmp	#ATEOL 	  	; Atari-EOL?
	beq	newline

	tay
	rol	a
	rol	a
	rol	a
	rol	a
	and	#3
	tax
	tya
	and	#$9f
	ora	ataint,x

cputdirect:			; accepts screen code
	jsr	putchar

	; update cursor position pointer
	ldy	#0
	lda	OLDCHR
	sta	(OLDADR),y
	inc	OLDADR
	bne	L1
	inc	OLDADR+1
L1:	lda	(OLDADR),y
	sta	OLDCHR
	ora	_revflag
	sta	(OLDADR),y
	
; advance cursor
	inc	COLCRS
	lda	COLCRS
	cmp	#40
	bcc	plot
	lda	#0
	sta	COLCRS

	.export newline
newline:
	inc	ROWCRS
	lda	ROWCRS
	cmp	#24
	bne	plot
	lda	#0
	sta	ROWCRS
plot:	ldy	COLCRS
	ldx	ROWCRS
	rts

; turn off cursor, update screen, turn on cursor
putchar:
	pha			; save char

	ldy	#0
	lda	OLDCHR
	sta	(OLDADR),y

	lda	ROWCRS
	jsr	mul40
L3:	clc
	adc	SAVMSC		; add start of screen memory
	sta	ptr4
	lda	tmp4
	adc	SAVMSC+1
	sta	ptr4+1
	pla			; get char again

	sta	OLDCHR

	ora	_revflag
	ldy	COLCRS
	sta	(ptr4),y

	; update OLDADR (maybe ROWCRS and COLCRS were changed)
	sty	tmp4
	lda	ptr4
	clc
	adc	tmp4
	sta	OLDADR
	lda	ptr4+1
	adc	#0
	sta	OLDADR+1
	rts

	.rodata
ataint:	.byte	64,0,32,96

