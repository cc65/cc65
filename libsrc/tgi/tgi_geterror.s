;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned char __fastcall__ tgi_geterror (void);
; /* Return the error code for the last operation. */

        .include        "tgi-kernel.inc"


_tgi_geterror:
        lda     _tgi_error
        rts

