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
        .import         _ria_call_int
        .constructor    _errno_opt_constructor

; The errno on the RIA is the errno for cc65
___errno        := RIA_ERRNO

.code

; Request the RIA use cc65 values for RIA_ERRNO
_errno_opt_constructor:
        lda #$01 ; 1 = cc65
        sta RIA_A
        lda #RIA_OP_ERRNO_OPT
        jmp _ria_call_int
