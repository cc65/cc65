;
; void* memset (void* ptr, int c, size_t n);
;
; Ullrich von Bassewitz, 29.05.1998
;

 	.export		_memset
	.import		popax
	.importzp	ptr1, ptr2, tmp1, tmp2, tmp3

_memset:
 	sta	tmp1		; Save n
 	stx	tmp2
 	jsr	popax  	 	; Get c
 	sta	tmp3		; Save c
 	jsr	popax  	 	; Get ptr
 	sta	ptr1
 	stx	ptr1+1 		; Save work copy
	sta	ptr2
	stx	ptr2+1		; Save a copy for the function result
       	lda	tmp3

	ldy	#0
	ldx	tmp2		; Get high byte of n
       	beq    	L2		; Jump if zero

; Set 256 byte blocks

L1:	sta	(ptr1),y	; Set one byte
  	iny
	sta	(ptr1),y	; Unroll this a bit to make it faster
	iny
  	bne	L1
	inc	ptr1+1
       	dex			; Next 256 byte block
	bne	L1		; Repeat if any

; Set the remaining bytes if any
					       
L2:    	ldx	tmp1		; Get the low byte of n
  	beq	L9		; Low byte is zero

L3:    	sta    	(ptr1),y       	; Set one byte
  	iny
       	dex			; Done?
  	bne	L3

L9:	lda	ptr2		; Load function result
	ldx	ptr2+1
   	rts


