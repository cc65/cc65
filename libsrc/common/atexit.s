;
; Ullrich von Bassewitz, 06.06.1998
;
; int atexit (void (*f) (void));
;

      	.export		_atexit
       	.destructor	doatexit, 16
	.import		__errno
 	.import	 	jmpvec

	.include	"errno.inc"

; ---------------------------------------------------------------------------

.proc	_atexit

	ldy	exitfunc_index
       	cpy    	#exitfunc_max		; Slot available?
       	beq    	@Error			; Jump if no

; Enter the function into the table

	sta	exitfunc_table,y
	iny
	txa
	sta	exitfunc_table,y
	iny
	sty	exitfunc_index

; Done, return zero

	lda	#0
	tax
	rts

; Error, no space left

@Error:	lda	#ENOSPC	      	; No space left
      	sta	__errno
      	ldx	#$00
      	stx	__errno+1
      	dex			; Make return value -1
      	txa
      	rts

.endproc



; ---------------------------------------------------------------------------

.code

.proc	doatexit

       	ldy    	exitfunc_index		; Get index
       	beq    	@L9	 		; Jump if done
 	dey
 	lda	exitfunc_table,y
   	sta	jmpvec+2
   	dey
   	lda	exitfunc_table,y
     	sta	jmpvec+1
   	sty	exitfunc_index
 	jsr	jmpvec			; Call the function
 	jmp	doatexit     		; Next one

@L9:	rts

.endproc



; ---------------------------------------------------------------------------

.bss
exitfunc_index:	.res	1	; Index into table, inc'ed by 2
exitfunc_table:	.res	10	; 5 exit functions
exitfunc_max	= <(* - exitfunc_table)


