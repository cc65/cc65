;
; 2003-08-12, Ullrich von Bassewitz
; 2015-09-24, Greg King
;
; Helper function for several high-level file functions.
;

        .include        "errno.inc"

; ----------------------------------------------------------------------------
; int __fastcall__ __directerrno (unsigned char code);
; /* Set errno to a specific error code, clear __oserror, and return -1. Used
; ** by the library.
; */

___directerrno:
        jsr     ___seterrno             ; Set errno (returns with .A = 0)
        sta     ___oserror              ; Clear ___oserror
.if .cap(CPU_HAS_INA)
        dec     a
.else
        lda     #$FF                    ; Return -1
.endif
        tax
        rts
