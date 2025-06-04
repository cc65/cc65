;
; Ullrich von Bassewitz, 26.05.2002
;
; CC65 runtime: zeropage usage
;

.include        "zeropage.inc"

; ------------------------------------------------------------------------

.zeropage

c_sp:             .res    2       ; Stack pointer
sreg:           .res    2       ; Secondary register/high 16 bit for longs
regsave:        .res    4       ; Slot to save/restore (E)AX into
ptr1:           .res    2
ptr2:           .res    2
ptr3:           .res    2
ptr4:           .res    2
tmp1:           .res    1
tmp2:           .res    1
tmp3:           .res    1
tmp4:           .res    1
regbank:        .res    regbanksize     ; Register bank
