;
; Ullrich von Bassewitz, 27.09.1998
;
; void set_brk (unsigned Addr);
; void reset_brk (void);
;

       	.export	   	_set_brk, _reset_brk
       	.export	       	_brk_a, _brk_x, _brk_y, _brk_sr, _brk_pc
	.import	   	_atexit

	.include   	"apple2.inc"

_brk_a = $45
_brk_x = $46
_brk_y = $47
_brk_sr = $48
_brk_sp = $49
_brk_pc = $3A

.bss
oldvec:       	.res	2   		; Old vector


.data
uservec:    	jmp	$FFFF		; Patched at runtime


.code

; Set the break vector
.proc	_set_brk

	sta	uservec+1
  	stx	uservec+2 	; Set the user vector

	lda	oldvec
	ora	oldvec+1	; Did we save the vector already?
       	bne	L1		; Jump if we installed the handler already

	lda	BRKVec
 	sta    	oldvec
 	lda 	BRKVec+1
 	sta	oldvec+1	; Save the old vector

	lda	#<_reset_brk
	ldx	#>_reset_brk
	jsr	_atexit		; Install an exit handler

L1:	lda	#<brk_handler	; Set the break vector to our routine
	sta	BRKVec
	lda	#>brk_handler
	sta	BRKVec+1
	rts

.endproc


; Reset the break vector
.proc	_reset_brk

	lda	oldvec
	sta	BRKVec
	lda	oldvec+1
	sta	BRKVec+1
	rts

.endproc



; Break handler, called if a break occurs

.proc	brk_handler

	sec
	lda	_brk_pc
	sbc	#$02		; Point to start of brk
	sta	_brk_pc
	lda	_brk_pc+1
	sbc	#$00
	sta	_brk_pc+1

	clc
	lda	_brk_sp
	adc	#$04		; Adjust stack pointer
	sta	_brk_sp

	lda	_brk_sr		; Clear brk
	and	#$EF
	sta	_brk_sr

	jsr    	uservec	       	; Call the user's routine

	lda	_brk_pc+1
	pha
	lda	_brk_pc
	pha
	lda	_brk_sr
	pha

	ldx	_brk_x
	ldy	_brk_y
	lda	_brk_a
	
	rti	     		; Jump back...

.endproc
	
