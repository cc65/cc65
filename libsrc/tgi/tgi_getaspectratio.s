;
; Ullrich von Bassewitz, 2011-05-01
;
; unsigned tgi_getaspectratio (void);
; /* Returns the aspect ratio for the loaded driver. The aspect ratio is an
; ** 8.8 fixed point value.
; */
;

        .include        "tgi-kernel.inc"

.proc   _tgi_getaspectratio

        lda     _tgi_aspectratio
        ldx     _tgi_aspectratio+1
        rts

.endproc
