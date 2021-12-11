
        .export         PLOT

        .include        "pce.inc"
        .include        "extzp.inc"

PLOT:   bcs     @getpos

        tya
        ;clc                    ; already cleared
        adc     plotlo,x
        sta     SCREEN_PTR

        cla
        adc     plothi,x
        sta     SCREEN_PTR+1
@getpos:
        ldx     CURS_Y
        ldy     CURS_X
        rts

.rodata

plotlo: .repeat screenrows,line
        .byte   <($0000+(line*$80))
        .endrepeat

plothi: .repeat screenrows,line
        .byte   >($0000+(line*$80))
        .endrepeat
