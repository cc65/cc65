;
; Ullrich von Bassewitz, 07.12.1998
;
; Copy the data segment from the LOAD to the RUN location
;

	.export		copydata
	.import		__DATA_LOAD__, __DATA_RUN__, __DATA_SIZE__
	.importzp	ptr1, ptr2


copydata:
	lda    	#<__DATA_LOAD__	; Source pointer
	sta	ptr1
	lda	#>__DATA_LOAD__
	sta	ptr1+1

	lda	#<__DATA_RUN__	; Target pointer
	sta	ptr2
	lda	#>__DATA_RUN__
	sta	ptr2+1

	ldy	#$00
	ldx	#>__DATA_SIZE__	; Get page count
	beq	@L2 		; No full pages

; Copy full pages

@L1:	lda	(ptr1),y
    	sta	(ptr2),y
    	iny
    	bne	@L1
	inc	ptr1+1
	inc	ptr2+2		; Bump pointers
    	dex
	bne	@L1

; Copy last page (remember: y contains zero)

@L2:	ldx	#<__DATA_SIZE__	; Get remaining bytes
    	beq	@L4
	   
@L3:	lda	(ptr1),y
    	sta	(ptr2),y
    	iny
    	dex
    	bne	@L3

; Done

@L4:	rts


