;
; IRQ handling (PCE version)
;

        .export         initirq, doneirq, IRQStub

        .import         __INTERRUPTOR_COUNT__, callirq_y

        .include        "pce.inc"
        .include        "extzp.inc"

; ------------------------------------------------------------------------
.segment        "INIT"

; a constructor
;
initirq:
        rts

; ------------------------------------------------------------------------
.code

; a destructor
;
doneirq:
        rts

; ------------------------------------------------------------------------

IRQStub:
        phy

; Save the display-source flags (and, release the interrupt).
;
        ldy     a:VDC_CTRL
        sty     vdc_flags

        ldy     #<(__INTERRUPTOR_COUNT__ * 2)
        beq     @L1
        phx
        pha

        jsr     callirq_y

        pla
        plx
@L1:    ply
        rti
