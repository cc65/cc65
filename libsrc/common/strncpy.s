;
; Ullrich von Bassewitz, 31.05.1998
;
; char* strncpy (char* dest, const char* src, unsigned size);
;

	.export		_strncpy
	.import		popax
	.importzp	ptr1, ptr2, ptr3, tmp1, tmp2

_strncpy:
	sta	tmp1		; Save size
       	stx	tmp2
       	jsr    	popax 	  	; get src
	sta	ptr1
	stx	ptr1+1
	jsr	popax 		; get dest
	sta	ptr2
	stx	ptr2+1
	sta	ptr3  		; remember for function return
	stx    	ptr3+1

	ldy	#$00
	ldx	tmp1		; Low byte of size
	beq	L1

; Copy the first chunk < 256

	jsr	CopyChunk
       	bcs 	L3		; Jump if end of string found

; Copy full 256 byte chunks

L1:    	lda	tmp2		; High byte of size
   	beq	L3
   	ldx	#$00		; Copy 256 bytes
L2:  	jsr	CopyChunk
   	bcs	L3
   	dec	tmp2
   	bne	L2
	beq	L9

; Fill the remaining space with zeroes. If we come here, the value in X
; is the low byte of the fill count, tmp2 holds the high byte. Y is the index
; into the target string.

L3:	tax			; Test low byte
	beq	L4
	jsr	FillChunk

L4:	lda	tmp2		; Test high byte
	beq	L9
L5:	jsr	FillChunk
   	dec	tmp2
   	bne	L5

; Done - return a pointer to the string

L9:	lda	ptr3
	ldx	ptr3+1
	rts


; -------------------------------------------------------------------
; Copy byte count in X from ptr1 to ptr2

.proc	CopyChunk
L1:	lda	(ptr1),y
	sta	(ptr2),y
	beq	L3
	iny
	bne	L2
	inc	ptr1+1
	inc	ptr2+1
L2:	dex
	bne	L1
	clc
	rts
L3:	sec
	rts
.endproc


; -------------------------------------------------------------------
; Fill byte count in X with zeroes

.proc	FillChunk
	lda	#$00
L1:	sta	(ptr1),y
	iny
	bne	L2
   	inc	ptr1+1
L2:	dex
	bne	L1
	rts
.endproc


