;
; void __fastcall__ _sys (struct regs* r);
;
; Ullrich von Bassewitz, 16.12.1998
;
;
; The function could also export jmpvec as general purpose jump vector that
; lies in the data segment so it's address may be patched at runtime.
;

  	.export		__sys
	.importzp	ptr1


__sys:	sta	ptr1
	stx	ptr1+1	      	; Save the pointer to r

; Fetch the PC and store it into the jump vector

	ldy	#5
	lda	(ptr1),y
	sta	jmpvec+2
	dey	
	lda	(ptr1),y
	sta	jmpvec+1

; Get the flags, mask unnecessary bits and push them. Push a

	dey
	lda	(ptr1),y
	and	#%11001011
	pha
	ldy	#0
	lda	(ptr1),y
	pha

; Get and assign X and Y

	iny
	lda	(ptr1),y
	tay
	iny
	lda	(ptr1),y
	tay

; Set a and the flags, call the machine code routine

	pla
	plp
	jsr	jmpvec

; Back from the routine. Save the flags and a

	php
	pha

; Put the register values into the regs structure

	tya
	ldy	#2
	sta	(ptr1),y
	dey
	txa
	sta	(ptr1),y
	dey
	pla
	sta	(ptr1),y
	ldy	#3
	pla
	sta	(ptr1),y

; Done

	rts

.data

jmpvec:	jmp	$FFFF

