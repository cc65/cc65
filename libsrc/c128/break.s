;
; Ullrich von Bassewitz, 27.09.1998
;
; void set_brk (unsigned Addr);
; void reset_brk (void);
;

       	.export	    	_set_brk, _reset_brk
  	.export	    	_brk_a, _brk_x, _brk_y, _brk_sr, _brk_pc
  	.import	    	_atexit
  	.importzp	ptr1

  	.include    	"c128.inc"


.bss
_brk_a:	 	.res	1
_brk_x:	 	.res   	1
_brk_y:	 	.res   	1
_brk_sr: 	.res	1
_brk_pc: 	.res	2

oldvec:       	.res	2   		; Old vector


.data
uservec:    	jmp 	$FFFF		; Patched at runtime

.code

; Where will we put the break stub?
stub_addr	= $0E00			; BASIC sprite area



; Set the break vector
.proc	_set_brk

	sta	uservec+1
  	stx	uservec+2     	; Set the user vector

	lda	oldvec
	ora	oldvec+1      	; Did we save the vector already?
       	bne    	L2    	      	; Jump if we installed the handler already

	lda	BRKVec
 	sta    	oldvec
 	lda 	BRKVec+1
 	sta	oldvec+1      	; Save the old vector

   	ldy	#stub_size-1	; Copy our stub into the low mem area
L1:	lda	brk_stub,y
	sta	stub_addr,y
	dey
	bpl	L1

	lda	#<_reset_brk
	ldx	#>_reset_brk
	jsr	_atexit	      	; Install an exit handler

L2:    	lda    	#<stub_addr 	; Set the break vector to our stub
  	sta	BRKVec
  	lda	#>stub_addr
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

	pla
	sta	_brk_y
	pla
	sta	_brk_x
	pla
	sta	_brk_a
	pla
	and	#$EF	 	; Clear break bit
	sta	_brk_sr
	pla	      	 	; PC low
	sec
	sbc	#2 	 	; Point to start of brk
	sta	_brk_pc
	pla	    	 	; PC high
	sbc	#0
	sta	_brk_pc+1

       	jsr	uservec	 	; Call the user's routine

	lda	_brk_pc+1
	pha
	lda	_brk_pc
	pha
	lda	_brk_sr
	pha
	ldx    	_brk_x
	ldy	_brk_y
	lda	_brk_a
	rti	   		; Jump back...

.endproc


brk_stub:
	.org	stub_addr
       	pla			; Get original MMU value
	sta	MMU_CR		; Re-enable our config
	jmp	brk_handler	; Jump to the user handler
	.reloc

stub_size	= * - brk_stub
