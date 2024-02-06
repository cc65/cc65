; Verifies that sim65 can handle 64-bit timeout counter.
; sim65 sim65-timeout.prg -x 4400000000

.export _main
.import exit

_main:
    ; wait ~4,500,000,000 cycles
    lda #45
    jsr wait100m
    ; This is a negative test.
    ; If the timeout did not occur, returning 0 reports failure.
    lda #0
    rts

; wait100m
.include "sim65-time-wait.inc"
