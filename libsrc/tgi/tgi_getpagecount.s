;
; Ullrich von Bassewitz, 23.06.2002
;
; unsigned tgi_getpagecount (void);
; /* Returns the number of screen pages available. */
;

        .include        "tgi-kernel.inc"

.proc   _tgi_getpagecount

        lda     _tgi_pagecount
        ldx     #0
        rts

.endproc
