;
; Mark Keates, Christian Groessler
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

    	.export	       	_cputcxy, _cputc
	.export		plot, cputdirect, putchar
	.import		popa, _gotoxy, mul40

	.include	"atari.inc"

_cputcxy:
	pha	    		; Save C
	jsr	popa		; Get Y
	jsr	_gotoxy		; Set cursor, drop x
	pla			; Restore C

.ifdef DIRECT_SCREEN

	.importzp tmp4,ptr4
	.import	_revflag

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

putchar:
	pha			; save char
	lda	ROWCRS
	jsr	mul40
L3:	clc
	adc	SAVMSC		; add start of screen memory
	sta	ptr4
	lda	tmp4
	adc	SAVMSC+1
	sta	ptr4+1
	pla			; get char again
	ora	_revflag
	ldy	COLCRS
	sta	(ptr4),y
	rts

	.rodata
ataint:	.byte	64,0,32,96

;****************************************************************
.else	;***** above DIRECT_SCREEN, below thru OS ***************
;****************************************************************

	.import		__do_oserror,cursor,__oserror


; Plot a character - also used as internal function

_cputc: cmp	#$0D  		; CR?
    	bne	L1
    	lda	#0
    	sta	COLCRS
       	beq    	plot		; Recalculate pointers

; don't know whether this is needed. the compiler generates
; already ATEOL chars for \n

L1: 	cmp	#$0A  	  	; LF?
       	bne	L2
	lda	#ATEOL

; Printable char of some sort

L2:
cputdirect:
	pha
	and	#$7f
	cmp	#32		; control char?
	bcs	goon
	lda	#$1b
	jsr	putchar
goon:	pla
	jsr	putchar		; Write the character to the screen

plot:	ldy	COLCRS
	ldx	ROWCRS
	rts

; Write one character to the screen without doing anything else, return X
; position in Y

putchar:
.if 0
	tax
	lda	#>(retr-1)
	pha
	lda	#<(retr-1)
	pha
	lda	ICPTH
	pha
	lda	ICPTL
	pha
	lda	#0
	sta	LOGCOL
	txa
	rts
retr:
.endif
.if 1
	pha
	ldx	#0		; iocb #0 (screen editor)
	txa
	sta	ICBLL,x
	sta	ICBLH,x
	sta	ICBAL,x
	sta	ICBAH,x
	lda	#PUTCHR
	sta	ICCOM,x
	lda	cursor
	beq	putc7
	lda	#0
	beq	putc8
putc7:	lda	#1
putc8:	sta	CRSINH
	pla
	jsr	CIOV
	bpl	putc9
	jmp	__do_oserror	; update system specific error code

putc9:	tya
	ldx	#0
	stx	__oserror
	ldy	COLCRS
.endif
	rts

.endif	; not defined DIRECT_SCREEN
