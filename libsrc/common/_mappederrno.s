;
; 2003-08-12, Ullrich von Bassewitz
; 2015-09-24, Greg King
;
; Helper function for several high-level file functions.
;

        .include        "errno.inc"

        .macpack        generic

; ----------------------------------------------------------------------------
; int __fastcall__ __mappederrno (unsigned char code);
; /* Set __oserror to the given platform-specific error code. If it is a real
; ** error code (not zero), set errno to the corresponding system error code,
; ** and return -1. Otherwise, return zero.
; ** Used by the library.
; */

___mappederrno:
        sta     ___oserror              ; Store the error code
        tax                             ; Did we have an error?
        bze     ok                      ; Branch if no
        jsr     ___osmaperrno           ; Map OS error into errno code
        jsr     ___seterrno             ; Save in errno (returns with .A = 0)
.if .cap(CPU_HAS_INA)
        dec     a
.else
        lda     #$FF                    ; Return -1 if error
.endif
        tax
ok:     rts
