;
; Ullrich von Bassewitz, 15.11.2001
;
; Allocate a block and zero it.
;
; void* __fastcall__ calloc (size_t count, size_t size);
;

  	.export	_calloc
	.import _malloc, _memset
	.import	tosumulax, pushax, push0


; -------------------------------------------------------------------------

.proc	_calloc

; We have the first argument in a/x and the second on the stack. Calling
; tosumulax will give the product of both in a/x.

  	jsr     tosumulax

; Save size for later

	sta	Size
	stx	Size+1

; malloc() is a fastcall function, so we do already have the argument in
; the right place

        jsr 	_malloc

; Check for a NULL pointer

  	cpx     #0
  	bne	ClearBlock
  	cmp	#0
  	beq	ClearBlock

; We have a NULL pointer, bail out

     	rts

; No NULL pointer, clear the block. memset will return a pointer to the
; block which is exactly what we want.

ClearBlock:
        jsr	pushax	     	      	; ptr
  	jsr	push0		      	; #0
	lda	Size
	ldx	Size+1			; Size
	jmp	_memset

.endproc

; -------------------------------------------------------------------------
; Data

.bss

Size:	.res	2



