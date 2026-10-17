;
; 2026, Rumbledethumps
;
; void __fastcall__ _xram1_poke16 (unsigned val, unsigned addr);

.include "xram.inc"

.export __xram1_poke16

.code

__xram1_poke16:
        xram_poke16 RIA_RW1, RIA_ADDR1, RIA_STEP1
