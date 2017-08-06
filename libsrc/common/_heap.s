;
; Ullrich von Bassewitz, 03.06.1998
;
; Heap variables and initialization.
;

        .constructor    initheap, 24
        .import         __BSS_RUN__, __BSS_SIZE__, __STACKSIZE__
        .importzp       sp

        .include        "_heap.inc"


.data

__heaporg:
        .word   __BSS_RUN__+__BSS_SIZE__        ; Linker calculates this symbol
__heapptr:
        .word   __BSS_RUN__+__BSS_SIZE__        ; Dito
__heapend:
        .word   __BSS_RUN__+__BSS_SIZE__
__heapfirst:
        .word   0
__heaplast:
        .word   0


; Initialization. Will be called from startup!

.segment        "ONCE"

initheap:
        sec
        lda     sp
        sbc     #<__STACKSIZE__
        sta     __heapend
        lda     sp+1
        sbc     #>__STACKSIZE__
        sta     __heapend+1
        rts

                      
