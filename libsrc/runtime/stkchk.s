;
; Ullrich von Bassewitz, 19.03.2001
;
; Stack checking code.
; For performance reasons (to avoid having to pass a parameter), the compiler
; calls the stkchk routine *after* allocating space on the stack. So the
; stackpointer may already be invalid if this routine is called. In addition
; to that, pushs and pops that are needed for expression evaluation are not
; checked (this would be way too much overhead). As a consequence we will
; operate using a safety area at the stack bottom. Once the stack reaches this
; safety area, we consider it an overflow, even if the stack is still inside
; its' bounds.
;

 	.export		stkchk
 	.constructor	initstkchk, 25
	.import		__STACKSIZE__			; Linker defined
       	.import	       	pusha0, exit
 	.importzp	sp

	; Use macros for better readability
	.macpack	generic

.code

; ----------------------------------------------------------------------------
; Initialization code. This is a constructor, so it is called on startup if
; the linker has detected references to this module.

.proc	initstkchk

	lda	sp
	sta	initialsp
	sub	#<__STACKSIZE__
	sta	lowwater
	lda	sp+1
	sta	initialsp+1
	sbc	#>__STACKSIZE__
	add	#1	 		; Add 256 bytes safety area
	sta	lowwater+1
	rts

.endproc

; ----------------------------------------------------------------------------
; Stack checking routine. Does not need to save any registers.

.proc	stkchk

	lda	lowwater+1
	cmp	sp+1
	bcs    	@L1
	rts

; Check low byte

@L1:	bne	@Overflow
	lda	lowwater
	cmp	sp
	bcs	@Overflow
	rts

; We have a stack overflow. Set the stack pointer to the low water mark, so
; we have

@Overflow:
 	lda	#4
 	jsr	pusha0
 	jmp	exit


.endproc

; ----------------------------------------------------------------------------
; Data

.bss

; Initial stack pointer value. Stack is reset to this in case of overflows to
; allow program exit processing.
initialsp: 	.word	0

; Stack low water mark.
lowwater:  	.word	0

	   
