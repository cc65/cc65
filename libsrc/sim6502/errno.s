;
; 2013-05-16, Oliver Schmidt
; 2015-07-18, Greg King
;
; Helper functions for several high-level functions.
;

        .include        "errno.inc"

; ----------------------------------------------------------------------------
; int __fastcall__ _directerrno (unsigned char code);
; /* Set errno to a specific error code; and, return -1. Used
; ** by the library.
; */

__directerrno:
        jsr     __seterrno              ; Save in errno
fail:   lda     #$FF                    ; Return -1
        tax
ok:     rts


; ----------------------------------------------------------------------------
;
; extern int _errno;
;
        .bss

__errno:
        .word   0
