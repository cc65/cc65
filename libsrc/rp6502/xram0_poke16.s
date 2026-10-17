;
; 2026, Rumbledethumps
;
; void __fastcall__ _xram0_poke16 (unsigned val, unsigned addr);

.include "xram.inc"

.export __xram0_poke16

.code

__xram0_poke16:
        xram_poke16 RIA_RW0, RIA_ADDR0, RIA_STEP0
