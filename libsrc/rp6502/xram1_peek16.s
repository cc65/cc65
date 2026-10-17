;
; 2026, Rumbledethumps
;
; unsigned __fastcall__ xram1_peek16 (unsigned addr);

.include "xram.inc"

.export _xram1_peek16

.code

_xram1_peek16:
        xram_peek16 RIA_RW1, RIA_ADDR1, RIA_STEP1
