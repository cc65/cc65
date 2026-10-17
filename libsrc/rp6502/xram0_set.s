;
; 2026, Rumbledethumps
;
; void __fastcall__ xram0_set (unsigned dest, unsigned char val, unsigned count);

.include "xram.inc"

.export _xram0_set

.code

_xram0_set:
        xram_set RIA_RW0, RIA_ADDR0, RIA_STEP0
