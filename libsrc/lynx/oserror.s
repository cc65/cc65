;
; Karri Kaksonen, 2010
;
; int __fastcall__ __osmaperrno (unsigned char oserror);
; /* Map a system specific error into a system independent code */
;

        .include        "errno.inc"

.code

___osmaperrno:
        rts

