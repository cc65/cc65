;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned __fastcall__ tgi_getxres (void);
; /* Return the resolution in X direction */


        .include        "tgi-kernel.inc"
        .export         _tgi_getxres


_tgi_getxres:
        lda     _tgi_xres
        ldx     _tgi_xres+1
        rts


