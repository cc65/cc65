;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned tgi_getxres (void);
; /* Return the resolution in X direction */


        .include        "tgi-kernel.inc"

.proc   _tgi_getxres

        lda     _tgi_xres
        ldx     _tgi_xres+1
        rts

.endproc
