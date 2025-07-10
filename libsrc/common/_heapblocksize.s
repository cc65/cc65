;
; Ullrich von Bassewitz, 2004-07-17
;
; size_t __fastcall__ _heapblocksize (const void* ptr);
;
; Return the size of an allocated block.
;

        .importzp       ptr1, ptr2
        .export         ___heapblocksize

        .include        "_heap.inc"

        .macpack        generic

;-----------------------------------------------------------------------------
; Code

___heapblocksize:

; Below the user data is a pointer that points to the start of the real
; (raw) memory block. The first word of this block is the size. To access
; the raw block pointer, we will decrement the high byte of the pointer,
; the pointer is then at offset 254/255.

        sta     ptr1
        dex
        stx     ptr1+1
        ldy     #$FE
        lda     (ptr1),y
        sta     ptr2            ; Place the raw block pointer into ptr2
        iny
        lda     (ptr1),y
        sta     ptr2+1

; Load the size from the raw block

        ldy     #usedblock::size+1
        lda     (ptr2),y
        tax
.if .cap(CPU_HAS_ZPIND)
        lda     (ptr2)
.else
        dey
        lda     (ptr2),y
.endif

; Correct the raw block size so that is shows the user visible portion. To
; do that, we must decrease the size by the amount of unused memory, which is
; the difference between the user space pointer and the raw memory block
; pointer. Since we have decremented the user space pointer by 256, we will
; have to correct the result.
;
;       return size - (ptr1 + 256 - ptr2)
;       return size - ptr1 - 256 + ptr2

        dex                     ; - 256
        add     ptr2
        pha
        txa
        adc     ptr2+1
        tax
        pla
        sub     ptr1
        pha
        txa
        sbc     ptr1+1
        tax
        pla

; Done

        rts

