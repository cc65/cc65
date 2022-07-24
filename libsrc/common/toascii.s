;
; unsigned char __fastcall__ toascii (unsigned char c);
; /* Convert a target specific character to ascii */
;

        .export         _toascii

_toascii:
        ldx     #>$0000
        rts
