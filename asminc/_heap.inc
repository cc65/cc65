;
; _heap.inc
;
; (C) Copyright 2003 Ullrich von Bassewitz (uz@cc65.org)
;

; Assembler include file that makes the constants and structures in _heap.h
; available for asm code.

HEAP_ADMIN_SPACE        =       2
HEAP_MIN_BLOCKSIZE      =       6       ; Minimum size of an allocated block

; Struct freeblock offsets and size. NOTE: For performance reasons, the asm
; code often uses increment/decrement operators to access other offsets, so
; just changing offsets here will probably not work.
freeblock_size  = 0
freeblock_next  = 2
freeblock_prev  = 4

; Variables
.global         __heaporg
.global         __heapptr
.global         __heapend
.global         __heapfirst
.global         __heaplast
