;
; IRQ handling (Lynx version)
;

        .export         initirq, doneirq
        .import         callirq

        .include        "lynx.inc"

; ------------------------------------------------------------------------

.segment        "ONCE"

initirq:
        lda     #<IRQStub
        ldx     #>IRQStub
        sei
        sta     INTVECTL
        stx     INTVECTH
        cli
        rts

; ------------------------------------------------------------------------

.code

doneirq:
        ; as Lynx is a console there is not much point in releasing the IRQ
        rts

; ------------------------------------------------------------------------

.segment        "LOWCODE"

IRQStub:
        phy
        phx
        pha
        jsr     callirq
        lda     INTSET
        sta     INTRST
        pla
        plx
        ply
        rti
