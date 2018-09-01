        .export         psg_init

        .include        "pce.inc"

        .segment        "ONCE"
psg_init:
        stz     PSG_GLOBAL_PAN          ; Silence global balance
        ldx     #6 - 1

psg_clear_loop:
        stx     PSG_CHAN_SELECT         ; Select channel
        stz     PSG_FREQ_LO             ; Clear frequency LSB
        stz     PSG_FREQ_HI             ; Clear frequency MSB
        stz     PSG_CHAN_CTRL           ; Clear volume
        stz     PSG_CHAN_PAN            ; Clear balance
        stz     PSG_NOISE               ; Clear noise control
        stz     PSG_LFO_FREQ            ; Clear LFO frequency
        stz     PSG_LFO_CTRL            ; Clear LFO control

        ldy     #$20
psg_clear_waveform:
        stz     PSG_CHAN_DATA           ; Clear waveform byte
        dey
        bne     psg_clear_waveform

        dex
        bpl     psg_clear_loop
        rts
