;
; Ullrich von Bassewitz, 06.06.1998
;
; int atexit (void (*f) (void));
;

      	.export		_atexit
	.import		exitfunc_table, exitfunc_index
	.importzp	exitfunc_max
	.import		__errno

	.include	"errno.inc"


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




