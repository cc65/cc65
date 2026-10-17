;
; 2026, Rumbledethumps
;
; unsigned char ria_vsync (void);

.include "rp6502.inc"

.export _ria_vsync

.code

_ria_vsync:
        lda     RIA_VSYNC
        ldx     #0
        rts
