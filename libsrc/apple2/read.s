;
; Ullrich von Bassewitz, 30.05.1998
;
; int read (int fd, void* buf, int count);
;
; THIS IS A HACK!
;

   	.export	      	_read
	.import	      	popax, _cputc
	.importzp     	ptr1, ptr2, ptr3

	.include       	"apple2.inc"

_read:	jsr	popax 		; get count
     	sta	ptr2
     	stx	ptr2+1		; save it for later
     	jsr	popax 		; get buf
     	sta	ptr1
     	stx	ptr1+1
     	jsr	popax 		; get fd and discard it
   	lda	#0
   	sta	ptr3
   	sta	ptr3+1		; set count

L1:    	lda	ptr2
   	ora	ptr2+1		; count zero?
   	beq	L9
   	jsr	RDKEY
	and	#$7f		; clear high bit.
	pha
	jsr	_cputc
	pla
	ldy	#0		; offset into string
   	sta	(ptr1),y	; save char
   	inc	ptr1
   	bne	L2
   	inc	ptr1+1
L2:    	inc    	ptr3  		; increment count
	bne	L3
	inc	ptr3+1
L3:	cmp	#$0D 		; CR?
	bne    	L1

; Done, return the count

L9:     lda	ptr3
      	ldx	ptr3+1
      	rts



