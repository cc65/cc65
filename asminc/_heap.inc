;
; _heap.inc
;
; (C) Copyright 2003 Ullrich von Bassewitz (uz@cc65.org)
;

; Assembler include file that makes the constants and structures in _heap.h
; available for asm code.

; Struct freeblock
; NOTE: For performance reasons, the asm code often uses increment/decrement
; operators to access other offsets, so just changing offsets here will
; probably not work.
.struct freeblock
        size    .word
        next    .word
        prev    .word
.endstruct

HEAP_ADMIN_SPACE    = 2
HEAP_MIN_BLOCKSIZE  = .sizeof (freeblock)   ; Minimum size of an allocated block

; Variables
.global         __heaporg
.global         __heapptr
.global         __heapend
.global         __heapfirst
.global         __heaplast
