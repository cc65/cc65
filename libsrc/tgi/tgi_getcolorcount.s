;
; Ullrich von Bassewitz, 22.06.2002
;
; unsigned char __fastcall__ tgi_getcolorcount (void);
; /* Get the number of available colors */

        .include        "tgi-kernel.inc"
        .export         _tgi_getcolorcount


_tgi_getcolorcount:
        lda     _tgi_colorcount
        ldx     #0
        rts

