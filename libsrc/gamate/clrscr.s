
        .include        "gamate.inc"
        .include        "extzp.inc"

        .import plot
        .export _clrscr
_clrscr:
        ldy     #$0
rowloop:
        sty     LCD_X

        lda     #0
        sta     LCD_Y

        ldx     #$0
colloop:
        sta     LCD_DATA

        inx
        bne     colloop

        iny
        bne     rowloop

; Go to the home position.

        sta     CURS_X
        sta     CURS_Y
        jmp     plot

;-------------------------------------------------------------------------------
; force the init constructor to be imported

                .import initconio
conio_init      = initconio
