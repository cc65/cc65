;
; Ullrich von Bassewitz, 21.7.2000
;
; Add a block to the heap free list
;
; void __fastcall__ _heapadd (void* mem, size_t size);
;
;

        .importzp       ptr1, ptr2
        .import         popax
        .import         heapadd
        .export         ___heapadd

        .include        "_heap.inc"

        .macpack        generic

;-----------------------------------------------------------------------------
; Code

___heapadd:
        sta     ptr1            ; Store size in ptr1
        stx     ptr1+1
        jsr     popax           ; Get the block pointer
        sta     ptr2
        stx     ptr2+1          ; Store block pointer in ptr2

; Check if size is greater or equal than min_size. Otherwise we don't care
; about the block (this may only happen for user supplied blocks, blocks
; from the heap are always large enough to hold a freeblock structure).

        lda     ptr1            ; Load low byte
        ldx     ptr1+1          ; Load/check high byte
        bne     @L1
        cmp     #HEAP_MIN_BLOCKSIZE
        bcs     @L1

        rts                     ; Block not large enough

; The block is large enough. Set the size field in the block.

@L1:    ldy     #usedblock::size
        sta     (ptr2),y
        iny
        txa
        sta     (ptr2),y

; Call the internal function since variables are now setup correctly

        jmp     heapadd

