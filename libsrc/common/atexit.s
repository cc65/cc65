;
; Ullrich von Bassewitz, 06.06.1998
;
; int atexit (void (*f) (void));
;

; The exit functions

      	.export		_atexit, doatexit
	.import		__errno, jmpvec

.bss
ecount:	.byte	0		; Really an index, inc'ed by 2
efunc: 	.word  	0,0,0,0,0	; 5 exit functions
maxcount = * - efunc


.code

_atexit:
	ldy	ecount
	cpy	#maxcount	; slot available?
	beq	E0		; jump if no

; Enter the function into the table

	sta	efunc,y
	iny
	txa
	sta	efunc,y
	iny
	sty	ecount

; Done, return zero

	lda	#0
	tax
	rts

; Error, no space left

E0:	lda	#$FF
       	sta    	__errno		; Use -1 until codes are defined ###
	sta	__errno+1
	tax
	rts

; Function called from exit

doatexit:
	ldy	ecount		; get index
	beq	L9		; jump if done
	dey
	lda	efunc,y
   	sta	jmpvec+2
   	dey
   	lda	efunc,y
     	sta	jmpvec+1
   	sty	ecount
	ldy	#0 		; number of function parms
	jsr	jmpvec
	jmp	doatexit     	; next one

L9:	rts





