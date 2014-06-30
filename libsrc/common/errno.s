;
; Ullrich von Bassewitz, 2003-08-12
;
; Helper functions for several high level file functions.
;


        .include        "errno.inc"

.code

; ----------------------------------------------------------------------------
; int __fastcall__ _directerrno (unsigned char code);
; /* Set errno to a specific error code, clear _oserror and return -1. Used
; ** by the library.
; */

__directerrno:
        jsr     __seterrno              ; Set errno, returns with A = 0
        sta     __oserror               ; Clear __oserror
        beq     fail                    ; Branch always

; ----------------------------------------------------------------------------
; int __fastcall__ _mappederrno (unsigned char code);
; /* Set _oserror to the given platform specific error code. If it is a real
; ** error code (not zero) set errno to the corresponding system error code
; ** and return -1. Otherwise return zero.
; ** Used by the library.
; */

__mappederrno:
        sta     __oserror               ; Store the error code
        tax                             ; Did we have an error?
        beq     ok                      ; Branch if no
        jsr     __osmaperrno            ; Map os error into errno code
        jsr     __seterrno              ; Save in errno
fail:   lda     #$FF                    ; Return -1
        tax
ok:     rts


; ----------------------------------------------------------------------------
.bss

__errno:
        .word   0

