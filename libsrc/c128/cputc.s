;
; Ullrich von Bassewitz, 06.08.1998
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

    	.export		_cputcxy, _cputc, cputdirect, putchar
	.export		plot
	.import		popa, _gotoxy
	.import		xsize, revers

	.include	"c128.inc"
	.include	"../cbm/cbm.inc"

_cputcxy:
	pha	    		; Save C
	jsr	popa		; Get Y
	jsr	_gotoxy		; Set cursor, drop x
	pla			; Restore C

; Plot a character - also used as internal function

_cputc: cmp    	#$0A  		; CR?
    	bne	L1
    	lda	#0
    	sta	CURS_X
       	beq    	plot		; Recalculate pointers

L1: 	cmp	#$0D  	  	; LF?
       	bne	L2
	ldy	CURS_Y
	iny
	bne	newline		; Recalculate pointers

; Printable char of some sort

L2:    	cmp	#' '
    	bcc	cputdirect	; Other control char
    	tay
    	bmi	L10
    	cmp	#$60
    	bcc	L3
    	and	#$DF
    	bne	cputdirect	; Branch always
L3: 	and	#$3F

cputdirect:
	jsr	putchar		; Write the character to the screen

; Advance cursor position

advance:
   	iny
   	cpy	xsize
   	bne	L9
newline:
   	ldy	#0    	  	; new line
   	clc
   	lda	xsize
   	adc	SCREEN_PTR
   	sta	SCREEN_PTR
   	bcc	L4
   	inc	SCREEN_PTR+1
	clc
L4:	lda    	xsize
   	adc	CRAM_PTR
   	sta	CRAM_PTR
   	bcc	L5
   	inc	CRAM_PTR+1
L5:	inc	CURS_Y
L9:    	sty	CURS_X
   	rts

; Handle character if high bit set

L10:	and	#$7F
       	cmp    	#$7E 	  	; PI?
	bne	L11
	lda	#$5E 	  	; Load screen code for PI
	bne	cputdirect
L11:	ora	#$40
	bne	cputdirect



; Set cursor position, calculate RAM pointers

plot:	ldy	CURS_X
	ldx	CURS_Y
	clc
	jmp	PLOT		; Set the new cursor



; Write one character to the screen without doing anything else, return X
; position in Y

putchar:
    	ora	revers	  	; Set revers bit
       	ldy    	CURS_X
	sta	(SCREEN_PTR),y	; Set char
	lda	CHARCOLOR
	sta	(CRAM_PTR),y	; Set color
	rts
