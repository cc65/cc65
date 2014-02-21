;
; Ullrich von Bassewitz, 2003-02-01
;
; Return the size of the largest free block on the heap.
;
; size_t _heapmaxavail (void);
;
;
                            
        .importzp       ptr1, ptr2
        .export         __heapmaxavail

        .include        "_heap.inc"

        .macpack        generic

;-----------------------------------------------------------------------------
; Code

__heapmaxavail:

; size_t Size = (_heapend - _heapptr) * sizeof (*_heapend);

        lda     __heapend
        sub     __heapptr
        sta     ptr2
        lda     __heapend+1
        sbc     __heapptr+1
        sta     ptr2+1

; struct freeblock* F = _heapfirst;

        lda     __heapfirst
        sta     ptr1
        lda     __heapfirst+1
@L1:    sta     ptr1+1

; while (F) {

        ora     ptr1
        beq     @L3             ; Jump if end of free list reached

; if (Size < F->size) {

        ldy     #freeblock::size
        lda     ptr2
        sub     (ptr1),y
        iny
        lda     ptr2+1
        sbc     (ptr1),y
        bcs     @L2

; Size = F->size;

        ldy     #freeblock::size
        lda     (ptr1),y
        sta     ptr2
        iny
        lda     (ptr1),y
        sta     ptr2+1

; F = F->next;

@L2:    iny                     ; Points to F->next
        lda     (ptr1),y
        tax
        iny
        lda     (ptr1),y
        stx     ptr1
        jmp     @L1

; if (Size < HEAP_ADMIN_SPACE) return 0;

@L3:    lda     ptr2
        sub     #HEAP_ADMIN_SPACE
        ldx     ptr2+1
        bcs     @L5
        bne     @L4
        txa
        rts

; return Size - HEAP_ADMIN_SPACE;

@L4:    dex
@L5:    rts
