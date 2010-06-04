;
; Ullrich von Bassewitz, 2003-08-12
;
; Helper function for several high level file functions.
;
; The function will store the value in A into _oserror. If the value is not
; zero, it is translated into a standard error number which is then stored
; into errno, and -1 is returned in a/x. If the value in A was zero, errno
; is not changed, and zero is returned in a/x.
;

        .export         oserrcheck

        .include        "errno.inc"

.proc   oserrcheck

        sta     __oserror               ; Store the error code
        tax                             ; Did we have an error?
        beq     ok                      ; Branch if no
        jsr     __osmaperrno            ; Map os error into errno code
        jsr     __seterrno              ; Save in errno
        lda     #$FF                    ; Return -1
        tax                             ; Make high byte also zero
ok:     rts

.endproc



