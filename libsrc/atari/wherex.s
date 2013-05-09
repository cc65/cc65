;
; Carsten Strotmann, 30.12.2002
;
; unsigned char wherex (void);
;

        .export  _wherex
        .include "atari.inc"

_wherex:
        lda     COLCRS
        ldx     #0
        rts
