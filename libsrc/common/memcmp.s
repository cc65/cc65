;
; Ullrich von Bassewitz, 09.06.1998
;
; int memcmp (const void* p1, const void* p2, size_t count);
;

	.export		_memcmp
	.import		popax, return0
	.importzp	ptr1, ptr2, ptr3

_memcmp:
    	sta	ptr3		; Save count
    	sta	ptr3+1
       	jsr	popax 	  	; get p2
    	sta	ptr2
    	stx	ptr2+1
    	jsr	popax 		; get p1
    	sta	ptr1
    	stx	ptr1+1

    	ldy	#0
    	ldx	ptr3		; use X as low counter byte
    	beq	L3

L1: 	lda	(ptr1),y
    	cmp	(ptr2),y
    	bne    	L5
    	iny
    	bne	L2
    	inc	ptr1+1
    	inc	ptr2+1
L2:    	txa
  	beq	L3
	dex
	jmp	L1
L3:	lda	ptr3+1		; check high byte
	beq	L4
	dec	ptr3+1
	dex			; X = $FF
	bne	L1		; branch always

; Memory areas are equal

L4:	jmp	return0

; Not equal, check which one is greater

L5:	bcs	L6
  	ldx	#$FF
  	rts

L6:	ldx	#$01
	rts

