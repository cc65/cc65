;
; Ullrich von Bassewitz, 2004-07-17
;
; size_t __fastcall__ _heapblocksize (const void* ptr);
;
; Return the size of an allocated block.
;

	.importzp    	ptr1
       	.export	     	__heapblocksize

        .include        "_heap.inc"

;-----------------------------------------------------------------------------
; Code

__heapblocksize:

; Decrement the block pointer so it points to the admin data

        sec
        sbc     #HEAP_ADMIN_SPACE       ; Assume it's less than 256
        bcs     L1
        dex
L1:     sta     ptr1
        stx     ptr1+1

; Load the size from the given block

        ldy     #1
        lda     (ptr1),y
        tax
        dey
        lda     (ptr1),y

; Done

        rts

