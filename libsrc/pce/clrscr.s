;
; Clear (erase) the screen.
;
; Support the full 128- x 64-tile background.
;

        .export         _clrscr

        .import         plot

        .include        "pce.inc"
        .include        "extzp.inc"

_clrscr:
        VREG    VDC_MAWR, $0000
        st0     #VDC_VWR

        ldy     #$40
rowloop:
        ldx     #$80
colloop:
        st1     #' '            ; low byte of char. index
        st2     #$02            ; background color, high nybble of char. index
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
