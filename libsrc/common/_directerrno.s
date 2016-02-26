;
; 2003-08-12, Ullrich von Bassewitz
; 2015-09-24, Greg King
;
; Helper function for several high-level file functions.
;

        .include        "errno.inc"

        .macpack        cpu

; ----------------------------------------------------------------------------
; int __fastcall__ _directerrno (unsigned char code);
; /* Set errno to a specific error code, clear _oserror, and return -1. Used
; ** by the library.
; */

__directerrno:
        jsr     __seterrno              ; Set errno (returns with .A = 0)
        sta     __oserror               ; Clear __oserror
.if (.cpu .bitand CPU_ISET_65SC02)
        dec     a
.else
        lda     #$FF                    ; Return -1
.endif
        tax
        rts
