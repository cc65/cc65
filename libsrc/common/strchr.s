;
; Ullrich von Bassewitz, 31.05.1998
;
; const char* strchr (const char* s, int c);
;

	.export		_strchr
	.import		popax
	.importzp	ptr1, tmp1

_strchr:
       	sta	tmp1		; Save c
       	jsr	popax 		; get s
       	sta	ptr1
       	stx	ptr1+1
       	ldy	#0

scloop:	lda	(ptr1),y	; get next char
       	beq	strz  		; jump if end of string
       	cmp	tmp1  		; found?
       	beq	strf  		; jump if yes
       	iny
       	bne	scloop
       	inc	ptr1+1
       	bne	scloop		; jump always

; found, calculate pointer to c

strf:  	ldx	ptr1+1		; get high byte of pointer
       	tya			; low byte offset
       	clc
       	adc	ptr1
       	bcc	str1
       	inx
str1:  	rts

; not found, return zero

strz:	tax			; return 0
	rts

