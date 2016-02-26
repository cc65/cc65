;
; 2003-08-12, Ullrich von Bassewitz
; 2015-09-24, Greg King
;
; extern int _errno;
; /* Library errors go here. */
;

        .include        "errno.inc"

.bss

__errno:
        .word   0
