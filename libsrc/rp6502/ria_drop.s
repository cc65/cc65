;
; 2026, Rumbledethumps
;
; void ria_drop (void);

.include "rp6502.inc"

.export _ria_drop

.code

_ria_drop:
        stz     RIA_OP          ; RIA_OP_DROP_XSTACK
        rts
