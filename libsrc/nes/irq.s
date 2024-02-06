;
; IRQ handling (NES version)
;

        .export         initirq, doneirq

; ------------------------------------------------------------------------

.segment        "ONCE"

initirq:
        rts

; ------------------------------------------------------------------------

.code

doneirq:
        rts
