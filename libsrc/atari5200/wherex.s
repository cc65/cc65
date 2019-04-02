;
; Carsten Strotmann, 30.12.2002
;
; unsigned char wherex (void);
;

        .export  _wherex
        .include "atari5200.inc"

_wherex:
        lda     COLCRS_5200
        ldx     #0
        rts
