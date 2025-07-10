;
; 2003-08-12, Ullrich von Bassewitz
; 2015-09-24, Greg King
;
; extern int __errno;
; /* Library errors go here. */
;

        .include        "errno.inc"

.bss

___errno:
        .word   0
