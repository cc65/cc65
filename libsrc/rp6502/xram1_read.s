;
; 2026, Rumbledethumps
;
; void __fastcall__ xram1_read (void* dest, unsigned src, unsigned count);

.include "xram.inc"

.export _xram1_read

.code

_xram1_read:
        xram_read RIA_RW1, RIA_ADDR1, RIA_STEP1
