;
; Ullrich von Bassewitz, 2003-02-01
;
; Return the amount of free memory on the heap.
;
; size_t _heapmemavail (void);
;
;

        .importzp       ptr1, ptr2
        .export         __heapmemavail

        .include        "_heap.inc"

        .macpack        generic

;-----------------------------------------------------------------------------
; Code

__heapmemavail:

; size_t Size = 0;

        lda     #0
        sta     ptr2
        sta     ptr2+1

; struct freeblock* F = _heapfirst;

        lda     __heapfirst
        sta     ptr1
        lda     __heapfirst+1
@L1:    sta     ptr1+1

; while (F) {

        ora     ptr1
        beq     @L2             ; Jump if end of free list reached

; Size += F->size;

        ldy     #freeblock::size
        lda     (ptr1),y
        add     ptr2
        sta     ptr2
        iny
        lda     (ptr1),y
        adc     ptr2+1
        sta     ptr2+1

; F = F->next;

        iny                             ; Points to F->next
        lda     (ptr1),y
        tax
        iny
        lda     (ptr1),y
        stx     ptr1
        jmp     @L1

; return Size + (_heapend - _heapptr) * sizeof (*_heapend);

@L2:    lda     ptr2
        add     __heapend
        sta     ptr2
        lda     ptr2+1
        adc     __heapend+1
        tax

        lda     ptr2
        sub     __heapptr
        sta     ptr2
        txa
        sbc     __heapptr+1
        tax
        lda     ptr2

        rts
