;
; IRQ handling (CreatiVision version)
;

        .export         initirq, doneirq
        .import         callirq, irq2

        .include        "creativision.inc"

; ------------------------------------------------------------------------

.segment        "ONCE"

initirq:
        lda     #<IRQStub
        ldx     #>IRQStub
        jmp     setvec

; ------------------------------------------------------------------------

.code

doneirq:
        lda     #<BIOS_IRQ2_ADDR
        ldx     #>BIOS_IRQ2_ADDR
setvec: sei
        sta     irq2+1
        stx     irq2+2
        cli
        rts

; ------------------------------------------------------------------------

.segment        "CODE"

IRQStub:
        cld                             ; Just to be sure
        jsr     callirq                 ; Call the functions
        jmp     BIOS_IRQ2_ADDR          ; Jump to the BIOS IRQ vector

