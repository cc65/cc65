;
; Ullrich von Bassewitz, 17.05.2000
;
; char* __fastcall__ strerror (int errcode);
; /* Map an error number to an error message */
;

        .export         _strerror
        .import         __sys_errlist

        .include        "errno.inc"

_strerror:
        cpx     #$00            ; High byte must be zero
        bne     @L1             ; Jump if invalid error
        cmp     #EMAX           ; Valid error code (map EUNKNOWN to 0)?
        bcc     @L2             ; Jump if ok

; The given error code is invalid

@L1:    lda     #<EINVAL
        sta     ___errno
        lda     #>EINVAL        ; = 0
        sta     ___errno+1
;       lda     #$00            ; A contains zero: "Unknown error"

; Load the pointer to the error message and return

@L2:    asl     a               ; * 2
        tay
        ldx     __sys_errlist+1,y
        lda     __sys_errlist,y
        rts


