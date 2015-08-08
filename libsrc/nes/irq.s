;
; IRQ handling (NES version)
;

        .export         initirq, doneirq

; ------------------------------------------------------------------------

.segment        "INIT"

initirq:
        rts

; ------------------------------------------------------------------------

.code

doneirq:
        rts
