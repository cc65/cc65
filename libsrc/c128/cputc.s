;
; Ullrich von Bassewitz, 06.08.1998
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

    	.export		_cputcxy, _cputc, cputdirect, putchar
	.export		newline, plot
        .constructor    initcputc
        .destructor     donecputc
	.import		popa, _gotoxy
        .import         PLOT

	.include	"c128.inc"


_cputcxy:
	pha	    		; Save C
	jsr	popa		; Get Y
	jsr	_gotoxy		; Set cursor, drop x
	pla			; Restore C
	jmp	PRINT

; Plot a character - also used as internal function

_cputc		= PRINT		; let the kernal handle it

cputdirect	= $c33b

newline:
	lda	#17
	jmp	PRINT

; Set cursor position, calculate RAM pointers

plot:	ldy	CURS_X
	ldx	CURS_Y
	clc
	jmp	PLOT		; Set the new cursor

; Write one character to the screen without doing anything else, return X
; position in Y

putchar	= $CC2F

;--------------------------------------------------------------------------
; Module constructor/destructor

initcputc:
	lda	#$80
        .byte   $2C
donecputc:
        lda     #$00
	sta	SCROLL
        rts

