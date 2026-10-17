;
; int __fastcall__ __directerrno (unsigned char code);
; /* Set errno to a specific error code and return -1. Used by the library. */
;
; Unlike the common version, this does not clear __oserror, because nothing on
; this target reads it.
;

        .include        "errno.inc"

___directerrno:
        jsr     ___seterrno             ; Returns with A = 0
        dec     a
        tax
        rts
