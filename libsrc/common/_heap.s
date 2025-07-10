;
; Ullrich von Bassewitz, 03.06.1998
;
; Heap variables and initialization.
;

        .constructor    initheap, 24
        .import         __BSS_RUN__, __BSS_SIZE__, __STACKSIZE__
        .importzp       c_sp

        .include        "_heap.inc"


.data

___heaporg:
        .word   __BSS_RUN__+__BSS_SIZE__        ; Linker calculates this symbol
___heapptr:
        .word   __BSS_RUN__+__BSS_SIZE__        ; Dito
___heapend:
        .word   __BSS_RUN__+__BSS_SIZE__
___heapfirst:
        .word   0
___heaplast:
        .word   0


; Initialization. Will be called from startup!

.segment        "ONCE"

initheap:
        sec
        lda     c_sp
        sbc     #<__STACKSIZE__
        sta     ___heapend
        lda     c_sp+1
        sbc     #>__STACKSIZE__
        sta     ___heapend+1
        rts


