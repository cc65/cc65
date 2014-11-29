;
; Ullrich von Bassewitz, 06.08.1998
;
; void cputcxy (unsigned char x, unsigned char y, char c);
; void cputc (char c);
;

    	.export	       	_cputcxy, _cputc, cputdirect, putchar
	.export		newline, plot
	.import		popa, _gotoxy
        .import         PLOT

		.importzp tmp3,tmp4

	.include	"pcengine.inc"


_cputcxy:
	pha	    		; Save C
	jsr	popa		; Get Y
	jsr	_gotoxy		; Set cursor, drop x
	pla			; Restore C

; Plot a character - also used as internal function

_cputc: cmp #$0d 		; CR?
    	bne	L1
    	lda	#0
    	sta	CURS_X
       	beq    	plot		; Recalculate pointers

L1: 	cmp	#$0a  	  	; LF?
       	beq	newline		; Recalculate pointers

; Printable char of some sort

cputdirect:
  	jsr	putchar		; Write the character to the screen

; Advance cursor position

advance:
	ldy	CURS_X
   	iny
   	cpy	#xsize
   	bne	L3
	jsr	newline		; new line
   	ldy	#0    	  	; + cr
L3:	sty	CURS_X
	jmp plot
   	;rts

newline:
;   	lda	#xsize
;   	clc
;   	adc	SCREEN_PTR
;   	sta	SCREEN_PTR
;   	bcc	L4
;   	inc	SCREEN_PTR+1
;;   	clc
;L4:;	lda #xsize
 ;  	adc	CRAM_PTR
 ;  	sta	CRAM_PTR
 ;  	bcc	L5
  ; 	inc	CRAM_PTR+1
;L5:
	
	inc	CURS_Y

;    jmp plot
;   	rts

; Set cursor position, calculate RAM pointers

plot:	ldy	CURS_X
		ldx	CURS_Y
		clc
		jmp	PLOT		; Set the new cursor



; Write one character to the screen without doing anything else, return X
; position in Y

putchar:

    	ora	RVS             ; Set revers bit

		tax

        st0     #VDC_MAWR    ; Memory Adress Write

		lda SCREEN_PTR
		staio VDC_DATA_LO

		lda SCREEN_PTR+1
		staio VDC_DATA_HI

		st0     #VDC_VWR    ; VWR

		txa
		staio VDC_DATA_LO   ; character

		;;st2     #$32      ; attrib ?!
		lda   CHARCOLOR
		
		;;lda #2
		asl a
		asl a
		asl a
		asl a

		and #$f0
        ora   #$02
		staio VDC_DATA_HI

        rts
