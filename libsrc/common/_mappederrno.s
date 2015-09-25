;
; 2003-08-12, Ullrich von Bassewitz
; 2015-09-24, Greg King
;
; Helper function for several high-level file functions.
;

        .include        "errno.inc"

        .macpack        generic
        .macpack        cpu

; ----------------------------------------------------------------------------
; int __fastcall__ _mappederrno (unsigned char code);
; /* Set _oserror to the given platform-specific error code. If it is a real
; ** error code (not zero), set errno to the corresponding system error code,
; ** and return -1. Otherwise, return zero.
; ** Used by the library.
; */

__mappederrno:
        sta     __oserror               ; Store the error code
        tax                             ; Did we have an error?
        bze     ok                      ; Branch if no
        jsr     __osmaperrno            ; Map OS error into errno code
        jsr     __seterrno              ; Save in errno (returns with .A = 0)
.if (.cpu .bitand CPU_ISET_65SC02)
        dec     a
.else
        lda     #$FF                    ; Return -1 if error
.endif
        tax
ok:     rts
