; 2022-01-17 Spiro Trikaliotis

.macro TESTER val
        lda #val
.endmacro

test:
        ldx #0
        TESTER 1
        ldx #15
        TESTER 2
        inx
        TESTER 3
        TESTER 4
        dex
        rts
