;
; Ullrich von Bassewitz, 17.7.2000
;
; Allocate a block from the heap.
;
; void* __fastcall__ malloc (size_t size);
;
;
; C implementation was:
;
; void* malloc (size_t size)
; /* Allocate memory from the given heap. The function returns a pointer to the
; ** allocated memory block or a NULL pointer if not enough memory is available.
; ** Allocating a zero size block is not allowed.
; */
; {
;     struct freeblock* f;
;     unsigned* p;
;
;
;     /* Check for a size of zero, then add the administration space and round
;     ** up the size if needed.
;     */
;     if (size == 0) {
;       return 0;
;     }
;     size += HEAP_ADMIN_SPACE;
;     if (size < sizeof (struct freeblock)) {
;         size = sizeof (struct freeblock);
;     }
;
;     /* Search the freelist for a block that is big enough */
;     f = _hfirst;
;     while (f && f->size < size) {
;         f = f->next;
;     }
;
;     /* Did we find one? */
;     if (f) {
;
;         /* We found a block big enough. If the block can hold just the
;         ** requested size, use the block in full. Beware: When slicing blocks,
;         ** there must be space enough to create a new one! If this is not the
;         ** case, then use the complete block.
;         */
;         if (f->size - size < sizeof (struct freeblock)) {
;
;             /* Use the actual size */
;             size = f->size;
;
;             /* Remove the block from the free list */
;             if (f->prev) {
;                 /* We have a previous block */
;                 f->prev->next = f->next;
;             } else {
;                 /* This is the first block, correct the freelist pointer */
;                 _hfirst = f->next;
;             }
;             if (f->next) {
;                 /* We have a next block */
;                 f->next->prev = f->prev;
;             } else {
;                 /* This is the last block, correct the freelist pointer */
;                 _hlast = f->prev;
;             }
;
;         } else {
;
;           /* We must slice the block found. Cut off space from the upper
;           ** end, so we can leave the actual free block chain intact.
;           */
;
;           /* Decrement the size of the block */
;           f->size -= size;
;
;           /* Set f to the now unused space above the current block */
;           f = (struct freeblock*) (((unsigned) f) + f->size);
;
;         }
;
;         /* Setup the pointer for the block */
;         p = (unsigned*) f;
;
;     } else {
;
;         /* We did not find a block big enough. Try to use new space from the
;         ** heap top.
;         */
;       if (((unsigned) _hend) - ((unsigned) _hptr) < size) {
;             /* Out of heap space */
;             return 0;
;       }
;
;
;       /* There is enough space left, take it from the heap top */
;       p = _hptr;
;               _hptr = (unsigned*) (((unsigned) _hptr) + size);
;
;     }
;
;     /* New block is now in p. Fill in the size and return the user pointer */
;     *p++ = size;
;     return p;
; }
;


        .importzp       ptr1, ptr2, ptr3
        .export         _malloc

        .include        "_heap.inc"

        .macpack        generic

;-----------------------------------------------------------------------------
; Code

_malloc:
        sta     ptr1                    ; Store size in ptr1
        stx     ptr1+1

; Check for a size of zero, if so, return NULL

        ora     ptr1+1
        beq     Done                    ; a/x already contains zero

; Add the administration space and round up the size if needed

        lda     ptr1
        add     #HEAP_ADMIN_SPACE
        sta     ptr1
        bcc     @L1
        inc     ptr1+1
        beq     OutOfHeapSpace          ; if high byte's 0, we overflowed!
@L1:    ldx     ptr1+1
        bne     @L2
        cmp     #HEAP_MIN_BLOCKSIZE+1
        bcs     @L2
        lda     #HEAP_MIN_BLOCKSIZE
        sta     ptr1                    ; High byte is already zero

; Load a pointer to the freelist into ptr2

@L2:    lda     ___heapfirst
        sta     ptr2
        lda     ___heapfirst+1
        sta     ptr2+1

; Search the freelist for a block that is big enough. We will calculate
; (f->size - size) here and keep it, since we need the value later.

        jmp     @L4

@L3:    ldy     #freeblock::size
        lda     (ptr2),y
        sub     ptr1
        tax                             ; Remember low byte for later
        iny                             ; Y points to freeblock::size+1
        lda     (ptr2),y
        sbc     ptr1+1
        bcs     BlockFound              ; Beware: Contents of a/x/y are known!

; Next block in list

        iny                             ; Points to freeblock::next
        lda     (ptr2),y
        tax
        iny                             ; Points to freeblock::next+1
        lda     (ptr2),y
        stx     ptr2
        sta     ptr2+1
@L4:    ora     ptr2
        bne     @L3

; We did not find a block big enough. Try to use new space from the heap top.

        lda     ___heapptr
        add     ptr1                    ; _heapptr + size
        tay
        lda     ___heapptr+1
        adc     ptr1+1
        bcs     OutOfHeapSpace          ; On overflow, we're surely out of space

        cmp     ___heapend+1
        bne     @L5
        cpy     ___heapend
@L5:    bcc     TakeFromTop
        beq     TakeFromTop

; Out of heap space

OutOfHeapSpace:
        lda     #0
        tax
Done:   rts

; There is enough space left, take it from the heap top

TakeFromTop:
        ldx     ___heapptr              ; p = _heapptr;
        stx     ptr2
        ldx     ___heapptr+1
        stx     ptr2+1

        sty     ___heapptr              ; _heapptr += size;
        sta     ___heapptr+1
        jmp     FillSizeAndRet          ; Done

; We found a block big enough. If the block can hold just the
; requested size, use the block in full. Beware: When slicing blocks,
; there must be space enough to create a new one! If this is not the
; case, then use the complete block.
; On input, x/a do contain the remaining size of the block. The zero
; flag is set if the high byte of this remaining size is zero.

BlockFound:
        bne     SliceBlock              ; Block is large enough to slice
        cpx     #HEAP_MIN_BLOCKSIZE     ; Check low byte
        bcs     SliceBlock              ; Jump if block is large enough to slice

; The block is too small to slice it. Use the block in full. The block
; does already contain the correct size word, all we have to do is to
; remove it from the free list.

        ldy     #freeblock::prev+1      ; Load f->prev
        lda     (ptr2),y
        sta     ptr3+1
        dey
        lda     (ptr2),y
        sta     ptr3
        dey                             ; Points to freeblock::next+1
        ora     ptr3+1
        beq     @L1                     ; Jump if f->prev zero

; We have a previous block, ptr3 contains its address.
; Do f->prev->next = f->next

        lda     (ptr2),y                ; Load high byte of f->next
        sta     (ptr3),y                ; Store high byte of f->prev->next
        dey                             ; Points to next
        lda     (ptr2),y                ; Load low byte of f->next
        sta     (ptr3),y                ; Store low byte of f->prev->next
        jmp     @L2

; This is the first block, correct the freelist pointer
; Do _hfirst = f->next

@L1:    lda     (ptr2),y                ; Load high byte of f->next
        sta     ___heapfirst+1
        dey                             ; Points to next
        lda     (ptr2),y                ; Load low byte of f->next
        sta     ___heapfirst

; Check f->next. Y points always to next if we come here

@L2:    lda     (ptr2),y                ; Load low byte of f->next
        sta     ptr3
        iny                             ; Points to next+1
        lda     (ptr2),y                ; Load high byte of f->next
        sta     ptr3+1
        iny                             ; Points to prev
        ora     ptr3
        beq     @L3                     ; Jump if f->next zero

; We have a next block, ptr3 contains its address.
; Do f->next->prev = f->prev

        lda     (ptr2),y                ; Load low byte of f->prev
        sta     (ptr3),y                ; Store low byte of f->next->prev
        iny                             ; Points to prev+1
        lda     (ptr2),y                ; Load high byte of f->prev
        sta     (ptr3),y                ; Store high byte of f->prev->next
        jmp     RetUserPtr              ; Done

; This is the last block, correct the freelist pointer.
; Do _hlast = f->prev

@L3:    lda     (ptr2),y                ; Load low byte of f->prev
        sta     ___heaplast
        iny                             ; Points to prev+1
        lda     (ptr2),y                ; Load high byte of f->prev
        sta     ___heaplast+1
        jmp     RetUserPtr              ; Done

; We must slice the block found. Cut off space from the upper end, so we
; can leave the actual free block chain intact.

SliceBlock:

; Decrement the size of the block. Y points to size+1.

        dey                             ; Points to size
        lda     (ptr2),y                ; Low byte of f->size
        sub     ptr1
        sta     (ptr2),y
        tax                             ; Save low byte of f->size in X
        iny                             ; Points to size+1
        lda     (ptr2),y                ; High byte of f->size
        sbc     ptr1+1
        sta     (ptr2),y

; Set f to the space above the current block, which is the new block returned
; to the caller.

        txa                             ; Get low byte of f->size
        add     ptr2
        tax
        lda     (ptr2),y                ; Get high byte of f->size
        adc     ptr2+1
        stx     ptr2
        sta     ptr2+1

; Fill the size and start address into the admin space of the block
; (struct usedblock) and return the user pointer

FillSizeAndRet:
        ldy     #usedblock::size        ; p->size = size;
        lda     ptr1                    ; Low byte of block size
        sta     (ptr2),y
        iny                             ; Points to freeblock::size+1
        lda     ptr1+1
        sta     (ptr2),y

RetUserPtr:
        ldy     #usedblock::start       ; p->start = p
        lda     ptr2
        sta     (ptr2),y
        iny
        lda     ptr2+1
        sta     (ptr2),y

; Return the user pointer, which points behind the struct usedblock

        lda     ptr2                    ; return ++p;
        ldx     ptr2+1
        add     #HEAP_ADMIN_SPACE
        bcc     @L9
        inx
@L9:    rts
