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
; ** (and do nothing in this case).
; */
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
;         ** remove this block from the freelist.
;         */
;         if (f = _hlast) {
;             if (((int) f) + f->size == (int) _hptr) {
;                 /* Remove the last block */
;                 _hptr = (unsigned*) (((int) _hptr) - f->size);
;                 if (_hlast = f->prev) {
;                   /* Block before is now last block */
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
;               /* Not at heap top, enter the block into the free list */
;       _hadd (b, size);
;
;     }
; }
;

        .importzp       ptr1, ptr2, ptr3, ptr4
        .export         _free, heapadd

        .include        "_heap.inc"

        .macpack        generic

;-----------------------------------------------------------------------------
; Code

_free:  sta     ptr2
        stx     ptr2+1                  ; Save block

; Is the argument NULL? If so, bail out.

        ora     ptr2+1                  ; Is the argument NULL?
        bne     @L1                     ; Jump if no
        rts                             ; Bail out if yes

; There's a pointer below the user space that points to the real start of the
; raw block. We will decrement the high pointer byte and use an offset of 254
; to save some code. The first word of the raw block is the total size of the
; block. Remember the block size in ptr1.

@L1:    dec     ptr2+1                  ; Decrement high pointer byte
        ldy     #$FF
        lda     (ptr2),y                ; High byte of real block address
        tax
        dey
        lda     (ptr2),y
        stx     ptr2+1
        sta     ptr2                    ; Set ptr2 to start of real block

        ldy     #usedblock::size+1
        lda     (ptr2),y                ; High byte of size
        sta     ptr1+1                  ; Save it
        dey
        lda     (ptr2),y
        sta     ptr1

; Check if the block is on top of the heap

        add     ptr2
        tay
        lda     ptr2+1
        adc     ptr1+1
        cpy     ___heapptr
        bne     heapadd                 ; Add to free list
        cmp     ___heapptr+1
        bne     heapadd

; The pointer is located at the heap top. Lower the heap top pointer to
; release the block.

@L3:    lda     ptr2
        sta     ___heapptr
        lda     ptr2+1
        sta     ___heapptr+1

; Check if the last block in the freelist is now at heap top. If so, remove
; this block from the freelist.

        lda     ___heaplast
        sta     ptr1
        ora     ___heaplast+1
        beq     @L9                     ; Jump if free list empty
        lda     ___heaplast+1
        sta     ptr1+1                  ; Pointer to last block now in ptr1

        ldy     #freeblock::size
        lda     (ptr1),y                ; Low byte of block size
        add     ptr1
        tax
        iny                             ; High byte of block size
        lda     (ptr1),y
        adc     ptr1+1

        cmp     ___heapptr+1
        bne     @L9                     ; Jump if last block not on top of heap
        cpx     ___heapptr
        bne     @L9                     ; Jump if last block not on top of heap

; Remove the last block

        lda     ptr1
        sta     ___heapptr
        lda     ptr1+1
        sta     ___heapptr+1

; Correct the next pointer of the now last block

        ldy     #freeblock::prev+1      ; Offset of ->prev field
        lda     (ptr1),y
        sta     ptr2+1                  ; Remember f->prev in ptr2
        sta     ___heaplast+1
        dey
        lda     (ptr1),y
        sta     ptr2                    ; Remember f->prev in ptr2
        sta     ___heaplast
        ora     ___heaplast+1           ; -> prev == 0?
        bne     @L8                     ; Jump if free list not empty

; Free list is now empty (A = 0)

        sta     ___heapfirst
        sta     ___heapfirst+1

; Done

@L9:    rts

; Block before is now last block. ptr2 points to f->prev.

@L8:    lda     #$00
        dey                             ; Points to high byte of ->next
        sta     (ptr2),y
        dey                             ; Low byte of f->prev->next
        sta     (ptr2),y
        rts                             ; Done

; The block is not on top of the heap. Add it to the free list. This was
; formerly a separate function called __hadd that was implemented in C as
; shown here:
;
; void _hadd (void* mem, size_t size)
; /* Add an arbitrary memory block to the heap. This function is used by
; ** free(), but it does also allow usage of otherwise unused memory
; ** blocks as heap space. The given block is entered in the free list
; ** without any checks, so beware!
; */
; {
;     struct freeblock* f;
;     struct freeblock* left;
;     struct freeblock* right;
;
;     if (size >= sizeof (struct freeblock)) {
;
;       /* Set the admin data */
;       f = (struct freeblock*) mem;
;       f->size = size;
;
;       /* Check if the freelist is empty */
;       if (_hfirst == 0) {
;
;           /* The freelist is empty until now, insert the block */
;           f->prev = 0;
;           f->next = 0;
;           _hfirst = f;
;           _hlast  = f;
;
;       } else {
;
;           /* We have to search the free list. As we are doing so, we check
;           ** if it is possible to combine this block with another already
;           ** existing block. Beware: The block may be the "missing link"
;           ** between *two* other blocks.
;           */
;           left = 0;
;           right = _hfirst;
;           while (right && f > right) {
;               left = right;
;               right = right->next;
;           }
;
;
;           /* OK, the current block must be inserted between left and right (but
;           ** beware: one of the two may be zero!). Also check for the condition
;           ** that we have to merge two or three blocks.
;           */
;           if (right) {
;               /* Check if we must merge the block with the right one */
;                       if (((unsigned) f) + size == (unsigned) right) {
;                   /* Merge with the right block */
;                   f->size += right->size;
;                   if (f->next = right->next) {
;                               f->next->prev = f;
;                   } else {
;                       /* This is now the last block */
;                       _hlast = f;
;                   }
;               } else {
;                   /* No merge, just set the link */
;                   f->next = right;
;                   right->prev = f;
;               }
;           } else {
;               f->next = 0;
;               /* Special case: This is the new freelist end */
;               _hlast = f;
;           }
;           if (left) {
;               /* Check if we must merge the block with the left one */
;               if ((unsigned) f == ((unsigned) left) + left->size) {
;                   /* Merge with the left block */
;                   left->size += f->size;
;                   if (left->next = f->next) {
;                       left->next->prev = left;
;                   } else {
;                       /* This is now the last block */
;                       _hlast = left;
;                   }
;               } else {
;                   /* No merge, just set the link */
;                   left->next = f;
;                   f->prev = left;
;               }
;           } else {
;               f->prev = 0;
;               /* Special case: This is the new freelist start */
;               _hfirst = f;
;           }
;       }
;     }
; }
;
;
; On entry, ptr2 must contain a pointer to the block, which must be at least
; HEAP_MIN_BLOCKSIZE bytes in size, and ptr1 contains the total size of the
; block.
;

; Check if the free list is empty, storing _hfirst into ptr3 for later

heapadd:
        lda     ___heapfirst
        sta     ptr3
        lda     ___heapfirst+1
        sta     ptr3+1
        ora     ptr3
        bne     SearchFreeList

; The free list is empty, so this is the first and only block. A contains
; zero if we come here.

        ldy     #freeblock::next-1
@L2:    iny                             ; f->next = f->prev = 0;
        sta     (ptr2),y
        cpy     #freeblock::prev+1      ; Done?
        bne     @L2

        lda     ptr2
        ldx     ptr2+1
        sta     ___heapfirst
        stx     ___heapfirst+1          ; _heapfirst = f;
        sta     ___heaplast
        stx     ___heaplast+1           ; _heaplast = f;

        rts                             ; Done

; We have to search the free list. As we are doing so, check if it is possible
; to combine this block with another, already existing block. Beware: The
; block may be the "missing link" between two blocks.
; ptr3 contains _hfirst (the start value of the search) when execution reaches
; this point, Y contains size+1. We do also know that _heapfirst (and therefore
; ptr3) is not zero on entry.

SearchFreeList:
        lda     #0
        sta     ptr4
        sta     ptr4+1                  ; left = 0;
        ldy     #freeblock::next+1
        ldx     ptr3

@Loop:  lda     ptr3+1                  ; High byte of right
        cmp     ptr2+1
        bne     @L1
        cpx     ptr2
        beq     @L2
@L1:    bcs     CheckRightMerge

@L2:    stx     ptr4                    ; left = right;
        sta     ptr4+1

        dey                             ; Points to next
        lda     (ptr3),y                ; right = right->next;
        tax
        iny                             ; Points to next+1
        lda     (ptr3),y
        stx     ptr3
        sta     ptr3+1
        ora     ptr3
        bne     @Loop

; If we come here, the right pointer is zero, so we don't need to check for
; a merge. The new block is the new freelist end.
; A is zero when we come here, Y points to next+1

        sta     (ptr2),y                ; Clear high byte of f->next
        dey
        sta     (ptr2),y                ; Clear low byte of f->next

        lda     ptr2                    ; _heaplast = f;
        sta     ___heaplast
        lda     ptr2+1
        sta     ___heaplast+1

; Since we have checked the case that the freelist is empty before, if the
; right pointer is NULL, the left *cannot* be NULL here. So skip the
; pointer check and jump right to the left block merge

        jmp     CheckLeftMerge2

; The given block must be inserted between left and right, and right is not
; zero.

CheckRightMerge:
        lda     ptr2
        add     ptr1                    ; f + size
        tax
        lda     ptr2+1
        adc     ptr1+1

        cpx     ptr3
        bne     NoRightMerge
        cmp     ptr3+1
        bne     NoRightMerge

; Merge with the right block. Do f->size += right->size;

        ldy     #freeblock::size
        lda     ptr1
        add     (ptr3),y
        sta     (ptr2),y
        iny                             ; Points to size+1
        lda     ptr1+1
        adc     (ptr3),y
        sta     (ptr2),y

; Set f->next = right->next and remember f->next in ptr1 (we don't need the
; size stored there any longer)

        iny                             ; Points to next
        lda     (ptr3),y                ; Low byte of right->next
        sta     (ptr2),y                ; Store to low byte of f->next
        sta     ptr1
        iny                             ; Points to next+1
        lda     (ptr3),y                ; High byte of right->next
        sta     (ptr2),y                ; Store to high byte of f->next
        sta     ptr1+1
        ora     ptr1
        beq     @L1                     ; Jump if f->next zero

; f->next->prev = f;

        iny                             ; Points to prev
        lda     ptr2                    ; Low byte of f
        sta     (ptr1),y                ; Low byte of f->next->prev
        iny                             ; Points to prev+1
        lda     ptr2+1                  ; High byte of f
        sta     (ptr1),y                ; High byte of f->next->prev
        jmp     CheckLeftMerge          ; Done

; f->next is zero, this is now the last block

@L1:    lda     ptr2                    ; _heaplast = f;
        sta     ___heaplast
        lda     ptr2+1
        sta     ___heaplast+1
        jmp     CheckLeftMerge

; No right merge, just set the link.

NoRightMerge:
        ldy     #freeblock::next        ; f->next = right;
        lda     ptr3
        sta     (ptr2),y
        iny                             ; Points to next+1
        lda     ptr3+1
        sta     (ptr2),y

        iny                             ; Points to prev
        lda     ptr2                    ; right->prev = f;
        sta     (ptr3),y
        iny                             ; Points to prev+1
        lda     ptr2+1
        sta     (ptr3),y

; Check if the left pointer is zero

CheckLeftMerge:
        lda     ptr4                    ; left == NULL?
        ora     ptr4+1
        bne     CheckLeftMerge2         ; Jump if there is a left block

; We don't have a left block, so f is actually the new freelist start

        ldy     #freeblock::prev
        sta     (ptr2),y                ; f->prev = 0;
        iny
        sta     (ptr2),y

        lda     ptr2                    ; _heapfirst = f;
        sta     ___heapfirst
        lda     ptr2+1
        sta     ___heapfirst+1

        rts                             ; Done

; Check if the left block is adjacent to the following one

CheckLeftMerge2:
        ldy     #freeblock::size        ; Calculate left + left->size
        lda     (ptr4),y                ; Low byte of left->size
        add     ptr4
        tax
        iny                             ; Points to size+1
        lda     (ptr4),y                ; High byte of left->size
        adc     ptr4+1

        cpx     ptr2
        bne     NoLeftMerge
        cmp     ptr2+1
        bne     NoLeftMerge             ; Jump if blocks not adjacent

; Merge with the left block. Do left->size += f->size;

        dey                             ; Points to size
        lda     (ptr4),y
        add     (ptr2),y
        sta     (ptr4),y
        iny                             ; Points to size+1
        lda     (ptr4),y
        adc     (ptr2),y
        sta     (ptr4),y

; Set left->next = f->next and remember left->next in ptr1.

        iny                             ; Points to next
        lda     (ptr2),y                ; Low byte of f->next
        sta     (ptr4),y
        sta     ptr1
        iny                             ; Points to next+1
        lda     (ptr2),y                ; High byte of f->next
        sta     (ptr4),y
        sta     ptr1+1
        ora     ptr1                    ; left->next == NULL?
        beq     @L1

; Do left->next->prev = left

        iny                             ; Points to prev
        lda     ptr4                    ; Low byte of left
        sta     (ptr1),y
        iny
        lda     ptr4+1                  ; High byte of left
        sta     (ptr1),y
        rts                             ; Done

; This is now the last block, do _heaplast = left

@L1:    lda     ptr4
        sta     ___heaplast
        lda     ptr4+1
        sta     ___heaplast+1
        rts                             ; Done

; No merge of the left block, just set the link. Y points to size+1 if
; we come here. Do left->next = f.

NoLeftMerge:
        iny                             ; Points to next
        lda     ptr2                    ; Low byte of left
        sta     (ptr4),y
        iny
        lda     ptr2+1                  ; High byte of left
        sta     (ptr4),y

; Do f->prev = left

        iny                             ; Points to prev
        lda     ptr4
        sta     (ptr2),y
        iny
        lda     ptr4+1
        sta     (ptr2),y
        rts                             ; Done







