        .include        "pce.inc"

        .export         psg_init

        .segment        "ONCE"
psg_init:
        clx
        stz     PSG_GLOBAL_PAN          ; Clear global balance

psg_clear_loop:
        stx     PSG_CHAN_SELECT         ; Select channel
        stz     PSG_FREQ_LO             ; Clear frequency LSB
        stz     PSG_FREQ_HI             ; Clear frequency MSB
        stz     PSG_CHAN_CTRL           ; Clear volume
        stz     PSG_CHAN_PAN            ; Clear balance
        stz     PSG_NOISE               ; Clear noise control
        stz     PSG_LFO_FREQ            ; Clear LFO frequency
        stz     PSG_LFO_CTRL            ; Clear LFO control

        cly
psg_clear_waveform:
        stz     PSG_CHAN_DATA           ; Clear waveform byte
        iny
        cpy     #$20
        bne     psg_clear_waveform

        inx
        cpx     #$06
        bne     psg_clear_loop
        rts
