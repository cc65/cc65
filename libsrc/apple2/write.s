	;;
	;; Kevin Ruland
	;;
	;; int write (int fd, const void* buf, int count);
	;;
	;; for now will only write to fd = stdout
	;;

	.export		_write

	.import		popax

	.importzp	ptr1, ptr2, ptr3

	.include	"apple2.inc"

.proc   _write

	sta	ptr2            ; Save count for later
	stx	ptr2+1
	sta	ptr3
	sta	ptr3+1		; save for result
	jsr	popax		; get buf
	sta	ptr1
	stx	ptr1+1
	jsr	popax		; get fd and discard
L1:	lda	ptr2
	ora	ptr2+1		; count zero?
	beq	L9
	ldy	#$00
	lda	(ptr1),y
	cmp	#$0A		; Check for \n = Crtl-j
	bne	rawout
	lda	#$0D		; Issue cr
rawout:
	ora	#$80
	jsr	COUT
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

.endproc

