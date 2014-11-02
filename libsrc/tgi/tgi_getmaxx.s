;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned tgi_getmaxx (void);
; /* Return the maximum x coordinate. The resolution in x direction is
; ** getmaxx() + 1
; */

        .include        "tgi-kernel.inc"


.proc   _tgi_getmaxx

        lda     _tgi_xmax
        ldx     _tgi_xmax+1
        rts

.endproc
