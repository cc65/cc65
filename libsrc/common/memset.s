;
; void* memset (void* ptr, int c, size_t n);
; void* _bzero (void* ptr, size_t n);
; void bzero (void* ptr, size_t n);
;
; Ullrich von Bassewitz, 29.05.1998
;
; NOTE: bzero will return it's first argument as memset does. It is no problem
;       to declare the return value as void, since it may be ignored. _bzero
;       (note the leading underscore) is declared with the proper return type,
;       because the compiler will replace memset by _bzero if the fill value
;       is zero, and the optimizer looks at the return type to see if the value
;       in a/x is of any use.
;

 	.export		_memset, _bzero, __bzero
	.import		popax
       	.importzp	sp, ptr1, ptr2, ptr3, tmp1

_bzero:
__bzero:
        sta     ptr3
        stx     ptr3+1          ; Save n
        lda     #0		; Fill with zeros
        beq     common
	
_memset:
 	sta	ptr3		; Save n
 	stx	ptr3+1
 	jsr	popax  	 	; Get c

; Common stuff for memset and bzero from here

common:	sta	tmp1		; Save the fill value
        ldy     #1
        lda     (sp),y
        tax
        dey
        lda     (sp),y          ; Get ptr
 	sta	ptr1
 	stx	ptr1+1 		; Save work copy

       	lda	tmp1            ; Load fill value
	ldy	#0
	ldx	ptr3+1  	; Get high byte of n
       	beq    	L2		; Jump if zero

; Set 256 byte blocks

L1:    	.repeat 2		; Unroll this a bit to make it faster
	sta	(ptr1),y	; Set one byte
  	iny
	.endrepeat
  	bne	L1
	inc	ptr1+1
       	dex			; Next 256 byte block
	bne	L1		; Repeat if any

; Set the remaining bytes if any

L2:    	ldx	ptr3		; Get the low byte of n
  	beq	L9		; Low byte is zero

L3:    	sta    	(ptr1),y       	; Set one byte
  	iny
       	dex			; Done?
  	bne	L3

L9:    	jmp     popax           ; Pop ptr and return as result


