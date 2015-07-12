
                .include "pce.inc"

                .export _clrscr
_clrscr:

                st0     #VDC_MAWR
                st1     #<$0000
                st2     #>$0000

                st0     #VDC_VWR
                ldy     #$40
rowloop:        ldx     #$80
colloop:        lda #' '
                staio VDC_DATA_LO
                lda #$02
                staio VDC_DATA_HI

                dex
                bne     colloop
                dey
                bne     rowloop

                rts

