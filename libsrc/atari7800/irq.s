;
; IRQ handling (Atari 7800 version)
;

        .export         initirq, doneirq, IRQStub

        .import         __INTERRUPTOR_COUNT__, callirq

        .include        "atari7800.inc"

	.code
; ------------------------------------------------------------------------

initirq:
doneirq:
        rts

; ------------------------------------------------------------------------

IRQStub:
        cld                             ; Just to be sure
        pha
        lda     #<(__INTERRUPTOR_COUNT__ * 2)
        beq     @L1
        txa
        pha
        tya
        pha
        jsr     callirq                 ; Call the functions
	pla
        tay
        pla
        tax
@L1:	pla
        rti

