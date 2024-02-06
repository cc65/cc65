;
; Carsten Strotmann, 30.12.2002
;
; unsigned char wherey (void);
;

        .export  _wherey
        .include "atari5200.inc"

_wherey:
        lda     ROWCRS_5200
        ldx     #0
        rts
