;
; 2003-08-12, Ullrich von Bassewitz
; 2015-09-24, Greg King
;
; extern int __errno;
; /* Library errors go here. */
;

        .include        "rp6502.inc"
        .include        "errno.inc"
        .export         ___errno
; This runs before the other library constructors, so OS calls made by
; them set cc65 errno values.
        .constructor    _errno_opt_constructor, 26

; The errno on the RIA is the errno for cc65
___errno        := RIA_ERRNO

.segment "ONCE"

; Request the RIA use cc65 values for RIA_ERRNO
_errno_opt_constructor:
        stz RIA_A ; 0 = RIA_ATTR_ERRNO_OPT
        lda #$01 ; 1 = cc65
        sta RIA_XSTACK
        lda #RIA_OP_ATTR_SET
        sta RIA_OP
        jmp RIA_SPIN
