;
; Ullrich von Bassewitz, 19.03.2000
;
; Free a block on the heap.
;
; void __fastcall__ free (void* block);
;
;
; C implementation was:
;
; void free (void* block)
; /* Release an allocated memory block. The function will accept NULL pointers
;  * (and do nothing in this case).
;  */
; {
;     unsigned* b;
;     unsigned size;
;     struct freeblock* f;
;
;
;     /* Allow NULL arguments */
;     if (block == 0) {
;         return;
;     }
;
;     /* Get a pointer to the real memory block, then get the size */
;     b = (unsigned*) block;
;     size = *--b;
;
;     /* Check if the block is at the top of the heap */
;     if (((int) b) + size == (int) _hptr) {
;
;         /* Decrease _hptr to release the block */
;         _hptr = (unsigned*) (((int) _hptr) - size);
;
;         /* Check if the last block in the freelist is now at heap top. If so,
;          * remove this block from the freelist.
;          */
;         if (f = _hlast) {
;             if (((int) f) + f->size == (int) _hptr) {
;                 /* Remove the last block */
;                 _hptr = (unsigned*) (((int) _hptr) - f->size);
;                 if (_hlast = f->prev) {
; 	       	    /* Block before is now last block */
;                     f->prev->next = 0;
;                 } else {
;                     /* The freelist is empty now */
;                     _hfirst = 0;
;                 }
;             }
;         }
;
;     } else {
;
;          	/* Not at heap top, enter the block into the free list */
;      	_hadd (b, size);
;
;     }
; }
;

	.importzp   	ptr1, ptr2
	.import		__hptr, __hfirst, __hlast
	.import	   	pushax, __hadd
	.export		_free

	.macpack	generic

; Offsets into struct freeblock and other constant stuff

size		= 0
next		= 2
prev		= 4
admin_space	= 2
min_size	= 6


; Code

_free:	sta	ptr1
	stx	ptr1+1	       	      	; Save block

; Is the argument NULL?

	ora 	ptr1+1 	       	      	; Is the argument NULL?
       	beq 	@L9 	       		; Jump if yes

; Decrement the given pointer by the admin space amount, so it points to the
; real block allocated. The size of the block is stored in the admin space.
; Remember the block size in ptr2.

	lda	ptr1
	sub	#admin_space
	sta	ptr1
    	bcs	@L1
    	dec	ptr1+1
@L1:	ldy	#size+1
	lda	(ptr1),y       	      	; High byte of size
	sta	ptr2+1 	       	      	; Save it
	dey
	lda	(ptr1),y
	sta	ptr2

; Check if the block is on top of the heap

	add	ptr1
	tay
	lda	ptr1+1
	adc	ptr2+1
	cpy	__hptr
	bne	@AddToFreeList
	cmp	__hptr+1
       	bne    	@AddToFreeList

; The pointer is located at the heap top. Lower the heap top pointer to
; release the block.

@L3:	lda	ptr1
	sta	__hptr
	lda	ptr1+1
	sta	__hptr+1

; Check if the last block in the freelist is now at heap top. If so, remove
; this block from the freelist.

	lda	__hlast
	sta	ptr2
	ora	__hlast+1
       	beq	@L9   	     		; Jump if free list empty
	lda	__hlast+1
	sta	ptr2+1 	     		; Pointer to last block now in ptr2

	ldy	#size
	lda	(ptr2),y     		; Low byte of block size
       	add	ptr2
	tax
	iny		    		; High byte of block size
	lda	(ptr2),y
	adc	ptr2+1

	cmp	__hptr+1
       	bne	@L9    	     		; Jump if last block not on top of heap
	cpx	__hptr
	bne	@L9    	     		; Jump if last block not on top of heap

; Remove the last block

	lda	ptr2
	sta	__hptr
	lda	ptr2+1
	sta	__hptr+1

; Correct the next pointer of the now last block

	ldy    	#prev+1	     		; Offset of ->prev field
       	lda    	(ptr2),y
	sta    	ptr1+1	    		; Remember f->prev in ptr1
	sta	__hlast+1
	dey
	lda	(ptr2),y
	sta	ptr1  	    		; Remember f->prev in ptr1
	sta	__hlast
    	ora	__hlast+1   		; -> prev == 0?
       	bne    	@L8    	    		; Jump if free list not empty

; Free list is now empty (A = 0)

	sta	__hfirst
	sta	__hfirst+1

; Done

@L9:	rts

; Block before is now last block. ptr1 points to f->prev.

@L8:	lda	#$00
    	dey	      	    		; Points to high byte of ->next
    	sta	(ptr1),y
    	dey	   	    		; Low byte of f->prev->next
    	sta	(ptr1),y
    	rts		    		; Done

; The block is not on top of the heap. Add it to the free list.

@AddToFreeList:
	lda    	ptr1
    	ldx	ptr1+1
    	jsr	pushax 	      	      	; Push b
    	lda	ptr2
    	ldx	ptr2+1
    	jsr	pushax 	      	      	; Push size
    	jmp	__hadd 	      	      	; Add to free list and return



			    
