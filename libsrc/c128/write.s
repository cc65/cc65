;
; Ullrich von Bassewitz, 30.05.1998
;
; int write (int fd, const void* buf, int count);
;
; THIS IS A HACK!
;

	.export		_write
	.import		popax
	.importzp	ptr1, ptr2, ptr3

	.include 	"../cbm/cbm.inc"

_write:	jsr	popax 		; get count
       	sta    	ptr2
	stx	ptr2+1		; save it for later
	sta	ptr3
	stx	ptr3+1		; save for function result
	jsr	popax		; get buf
	sta	ptr1
	stx	ptr1+1
	jsr	popax		; get fd and discard it

L1:	lda	ptr2
	ora	ptr2+1		; count zero?
	beq	L9
	ldy	#0
	lda	(ptr1),y
	jsr	BSOUT
	inc	ptr1
	bne	L2
	inc	ptr1+1
L2:	lda	ptr2
	bne	L3
	dec	ptr2
	dec	ptr2+1
	jmp	L1
L3:	dec	ptr2
	jmp	L1

; No error, return count

L9:   	lda	ptr3
    	ldx	ptr3+1
    	rts

