;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned char __fastcall__ tgi_geterror (void);
; /* Return the error code for the last operation. This will also clear the
;  * error.
;  */

        .include        "tgi-kernel.inc"

        .export         _tgi_geterror

_tgi_geterror:
        ldx     #0
        lda     _tgi_error
        stx     _tgi_error
        rts

