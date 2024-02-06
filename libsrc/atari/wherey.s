;
; Carsten Strotmann, 30.12.2002
;
; unsigned char wherey (void);
;

        .export  _wherey
        .include "atari.inc"

_wherey:
        lda     ROWCRS
        ldx     #0
        rts
