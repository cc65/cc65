;
; 2026, Rumbledethumps
;
; unsigned __fastcall__ xram0_peek16 (unsigned addr);

.include "xram.inc"

.export _xram0_peek16

.code

_xram0_peek16:
        xram_peek16 RIA_RW0, RIA_ADDR0, RIA_STEP0
