;
; Ullrich von Bassewitz, 22.06.2002
;
; unsigned char tgi_getcolorcount (void);
; /* Get the number of available colors */

        .include        "tgi-kernel.inc"

.proc   _tgi_getcolorcount

        lda     _tgi_colorcount
        ldx     #0
        rts

.endproc
