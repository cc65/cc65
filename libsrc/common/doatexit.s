;
; Ullrich von Bassewitz, 26.10.2000
;
; Handle exit functions
;

      	.export	       	doatexit, exitfunc_index, exitfunc_table
       	.exportzp      	exitfunc_max
 	.import	 	jmpvec

.bss
exitfunc_index:	.res	1	; Index into table, inc'ed by 2
exitfunc_table:	.res	10	; 5 exit functions
exitfunc_max	= <(* - exitfunc_table)


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



