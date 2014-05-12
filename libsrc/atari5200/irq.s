;
; IRQ handling (ATARI 5200 version)
;

        .export         initirq, doneirq
        .import         callirq

        .include        "atari5200.inc"

; Dummy versions for now

initirq:
doneirq:
        rts
