;
; Jede, 2017-10-27
;
; int __fastcall__ __osmaperrno (unsigned char oserror);
; /* Map a system specific error into a system independent code */
;

        .include        "errno.inc"
        .export         ___osmaperrno

.proc   ___osmaperrno

        lda     #<EUNKNOWN
        ldx     #>EUNKNOWN
        rts

.endproc
