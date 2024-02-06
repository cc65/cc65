; 2022-01-17 Spiro Trikaliotis

.macro TESTER
        lda #2
.endmacro

test:
        ldx #0
        TESTER
        ldx #15
        TESTER
        inx
        TESTER
        TESTER
        dex
        rts
