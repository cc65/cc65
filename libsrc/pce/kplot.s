
        .export         PLOT

        .include        "pce.inc"
        .include        "extzp.inc"

PLOT:
        bcs     @getpos

        tya
        ;clc                    ; already cleared
        adc     _plotlo,x
        sta     SCREEN_PTR

        lda     _plothi,x
        adc     #0
        sta     SCREEN_PTR+1
@getpos:
        ldx     CURS_Y
        ldy     CURS_X
        rts

        .rodata

_plotlo:
        .repeat screenrows,line
        .byte   <($0000+(line*$80))
        .endrepeat

_plothi:
        .repeat screenrows,line
        .byte   >($0000+(line*$80))
        .endrepeat

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import initconio
conio_init      = initconio
