;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned __fastcall__ tgi_getyres (void);
; /* Return the resolution in Y direction */


        .include        "tgi-kernel.inc"
        .export         _tgi_getyres
        .import         ldaxidx


_tgi_getyres:
        lda     _tgi_yres
        ldx     _tgi_yres+1
        rts


