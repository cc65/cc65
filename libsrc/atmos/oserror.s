;
; Stefan Haubenthal, 2011-04-18
;
; int __fastcall__ _osmaperrno (unsigned char oserror);
; /* Map a system specific error into a system independent code */
;

        .include        "errno.inc"
        .export         __osmaperrno

.proc   __osmaperrno

        lda     #<EUNKNOWN
        ldx     #>EUNKNOWN
        rts

.endproc
