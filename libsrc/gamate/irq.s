;
; IRQ handling (Gamate version)
;

        .export         initirq, doneirq, IRQStub

        .import         __INTERRUPTOR_COUNT__, callirq_y

        .include        "gamate.inc"
        .include        "extzp.inc"

; ------------------------------------------------------------------------
.segment        "ONCE"

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
; 256*32 interrupts in about 1minute 60s = 136hz
; -> guess 16384 clock cycles = 135,28hz (might be audio signal 1/512?)

IRQStub:
        ; A and Y are saved by the BIOS
        ;pha
        ;tya
        ;pha

        ldy     #<(__INTERRUPTOR_COUNT__ * 2)
        beq     @L1

        txa
        pha

        jsr     callirq_y

        pla
        tax

@L1:    ;pla
        ;tay
        ;pla
        rts
