;
; Ullrich von Bassewitz, 09.06.1998
;
; void* memchr (const void* p, int c, size_t n);
;

	.export		_memchr
	.import		popax, return0
	.importzp	ptr1, ptr2, tmp1


_memchr:
       	sta	ptr2		; Save n
	stx	ptr2+1
	jsr	popax		; get c
	sta	tmp1
       	jsr	popax 	     	; get p
       	sta	ptr1
       	stx	ptr1+1

       	ldy	#0
	lda	tmp1		; get c
	ldx	ptr2		; use X as low counter byte
	beq	L3		; check high byte

; Search for the char

L1:	cmp	(ptr1),y
       	beq    	L5  		; jump if found
   	iny
   	bne	L2
   	inc     ptr1+1
L2:	cpx	#0
   	beq	L3
   	dex
   	jmp	L1
L3:	ldx	ptr2+1		; Check high byte
   	beq	L4  		; Jump if counter off
   	dec	ptr2+1
   	ldx	#$FF
   	bne	L1		; branch always

; Character not found, return zero

L4:	jmp	return0

; Character found, calculate pointer

L5:  	ldx 	ptr1+1		; get high byte of pointer
       	tya 			; low byte offset
       	clc
       	adc 	ptr1
       	bcc 	L6
       	inx
L6:  	rts


