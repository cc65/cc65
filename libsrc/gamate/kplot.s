
        .export         PLOT

        .include        "gamate.inc"
        .include        "extzp.inc"

PLOT:
        bcs     @getpos

        sty     LCD_X
        ;clc                    ; already cleared
        lda     _plotlo,x
        sta     LCD_Y

@getpos:
        ldx     CURS_Y
        ldy     CURS_X
        rts

        .export         _plotlo
        .rodata

_plotlo:
        .repeat screenrows,line
        .byte   <($0000+(line*$8))
        .endrepeat

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import initconio
conio_init      = initconio
