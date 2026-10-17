;
; 2026, Rumbledethumps
;
; void __fastcall__ xram0_read (void* dest, unsigned src, unsigned count);

.include "xram.inc"

.export _xram0_read

.code

_xram0_read:
        xram_read RIA_RW0, RIA_ADDR0, RIA_STEP0
