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

        .macpack        generic
        .macpack        cpu

;-----------------------------------------------------------------------------
; Code

__heapblocksize:

; Decrement the block pointer so it points to the admin data

        sub     #HEAP_ADMIN_SPACE       ; Assume it's less than 256
        bcs     L1
        dex
L1:     sta     ptr1
        stx     ptr1+1

; Load the size from the given block

        ldy     #1
        lda     (ptr1),y
        tax
.if (.cpu .bitand CPU_ISET_65SC02)
        lda     (ptr1)
.else
        dey
        lda     (ptr1),y
.endif

; Adjust it to the user visible size

        sub     #HEAP_ADMIN_SPACE
        bcs     L9
        dex

; Done

L9:     rts

