;
; 2023, Rumbledethumps
;
; Enables the C IRQ tools

.export initirq, doneirq
.import callirq, _exit

.include "rp6502.inc"

.segment "ONCE"

initirq:
    lda #<handler
    ldx #>handler
    sta $FFFE
    stx $FFFF
    cli
    rts

.code

doneirq:
    sei
    rts

handler:
    phx
    tsx
    pha
    lda $0102,X
    and #$10
    bne break
    phy
    jsr callirq
    ply
    pla
    plx
    rti

break:
    lda #$FF
    ldx #0
    jmp _exit
