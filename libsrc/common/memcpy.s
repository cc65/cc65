;
; void* memcpy (void* dest, const void* src, size_t n);
; void* memmove (void* dest, const void* src, size_t n);
;
; Ullrich von Bassewitz, 10.12.1998
;

  	.export		_memcpy, _memmove
	.import		popax
	.importzp	ptr1, ptr2, ptr3, tmp1, tmp2

; ----------------------------------------------------------------------
_memcpy:
	jsr	getparms	; Get the parameters from stack

; Copy upwards

copyup:	ldy   	#0  		; set up to move 256
	ldx   	tmp2		; hi byte of n
  	beq   	@L2

@L1:	lda   	(ptr1),y	; get a byte
   	sta   	(ptr2),y	; store it
   	iny
       	bne    	@L1
   	inc   	ptr1+1		; bump ptrs
   	inc   	ptr2+1
       	dex
  	bne   	@L1  		; do another block

@L2:	ldx	tmp1		; get low byte of n
  	beq    	done		; jump if done

@L3:	lda	(ptr1),y	; get a byte
  	sta	(ptr2),y	; store it
  	iny
  	dex
  	bne	@L3

done:	lda	ptr3
  	ldx	ptr3+1		; get function result (dest)
       	rts


; ----------------------------------------------------------------------
_memmove:
  	jsr	getparms	; Get the parameters from stack

  	cpx	ptr1+1		; dest > src?
  	bne	@L1
  	cmp	ptr1
@L1:	beq	done		; Both pointers are equal - nothing to copy
  	bcc	copyup		; Copy upwards

; Copy downwards

	clc
       	lda	ptr1+1
	adc	tmp2
	sta	ptr1+1

	clc
	lda	ptr2+1
	adc	tmp2
	sta	ptr2+1

; Copy the incomplete page

    	ldy	tmp1  		; Get low byte of count
    	beq    	@L3

@L2:	dey
    	lda	(ptr1),y
    	sta	(ptr2),y
    	tya			; Test Y
    	bne	@L2		; Jump if not zero

; Copy complete pages

@L3:	ldx	tmp2		; Get hi byte of count
      	beq	done

@L4:  	dec	ptr1+1
      	dec	ptr2+1
@L5:  	dey
      	lda	(ptr1),y
      	sta	(ptr2),y
      	tya
      	bne	@L5
      	dex
      	bne	@L4

; Done

	beq	done

; ----------------------------------------------------------------------
; Get the parameters from stack
	   
getparms:
       	sta	tmp1		; Save n
       	stx	tmp2
       	jsr	popax		; src
       	sta	ptr1
       	stx	ptr1+1
       	jsr	popax		; dest
  	sta	ptr2
  	stx	ptr2+1		; save work copy
  	sta	ptr3
  	stx	ptr3+1		; save function result
	rts


