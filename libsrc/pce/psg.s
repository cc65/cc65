
                .include "pce.inc"

                .export psg_init

psg_init:
                clx
                stx     $0800   ; Select channel
psg_clear_loop:
                stz     $0801   ; Clear global balance
                stz     $0802   ; Clear frequency LSB
                stz     $0803   ; Clear frequency MSB
                stz     $0804   ; Clear volume
                stz     $0805   ; Clear balance
                stz     $0807   ; Clear noise control
                stz     $0808   ; Clear LFO frequency
                stz     $0809   ; Clear LFO control

                cly
psg_clear_waveform:     stz     $0806   ; Clear waveform byte
                iny
                cpy     #$20
                bne     psg_clear_waveform

                inx
                cpx     #$06
                bne     psg_clear_loop
                rts

