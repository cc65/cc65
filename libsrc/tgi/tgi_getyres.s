;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned tgi_getyres (void);
; /* Return the resolution in Y direction */


        .include        "tgi-kernel.inc"

.proc   _tgi_getyres

        lda     _tgi_yres
        ldx     _tgi_yres+1
        rts

.endproc
