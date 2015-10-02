        .export         _clrscr

        .import         plot

        .include        "pce.inc"
        .include        "extzp.inc"

_clrscr:
        st0     #VDC_MAWR
        st1     #<$0000
        st2     #>$0000

        st0     #VDC_VWR
        ldy     #$40
rowloop:
        ldx     #$80
colloop:
        lda     #' '
        sta     VDC_DATA_LO
        lda     #$02
        sta     VDC_DATA_HI

        dex
        bne     colloop
        dey
        bne     rowloop

; Go to the home position.

        stz     CURS_X
        stz     CURS_Y
        jmp     plot

;-------------------------------------------------------------------------------
; force the init constructor to be imported

                .import initconio
conio_init      = initconio
