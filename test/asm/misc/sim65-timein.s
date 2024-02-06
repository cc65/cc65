; Verifies that sim65 can handle 64-bit timeout counter.
; sim65 sim65-timein.prg -x 4400000000

.export _main
.import exit

_main:
    ; wait ~4,300,000,000 cycles
    lda #43
    jsr wait100m
    ; This is a positive test.
    ; If the timeout did not occur, returning 0 reports success.
    lda #0
    rts

; wait100m
.include "sim65-time-wait.inc"
