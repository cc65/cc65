;
; Ullrich von Bassewitz, 21.06.2002
;
; unsigned tgi_getmaxy (void);
; /* Return the maximum y coordinate. The resolution in y direction is
; ** getmaxy() + 1
; */

        .include        "tgi-kernel.inc"

.proc   _tgi_getmaxy

        lda     _tgi_ymax
        ldx     _tgi_ymax+1
        rts

.endproc
