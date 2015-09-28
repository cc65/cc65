
        .export         soft80_kplot

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_kplot:
        bcs     @getpos

        stx     CURS_Y
        sty     CURS_X

        ; calc pointer to bitmap
        lda     _bitmaplo,x
        clc
        adc     _bitmapxlo,y
        sta     SCREEN_PTR
        lda     _bitmaphi,x
        adc     _bitmapxhi,y
        sta     SCREEN_PTR+1

        ; calc pointer to vram
        tya
        lsr     a               ; NOTE: we can save 2 cycles here at the expense of
                                ;       making the tables twice as big (+50 bytes)
        clc
        adc     _vramlo,x
        sta     CRAM_PTR
        lda     #0
        adc     _vramhi,x
        sta     CRAM_PTR+1

@getpos:
        ldx     CURS_Y
        ldy     CURS_X
        rts

        .rodata
_bitmapxlo:
        .repeat 80,col
        .byte <((col/2)*8)
        .endrepeat

_bitmapxhi:
        .repeat 80,col
        .byte >((col/2)*8)
        .endrepeat
_vramlo:
        .repeat 25,row
        .byte <(soft80_vram+(row*40))
        .endrepeat
_vramhi:
        .repeat 25,row
        .byte >(soft80_vram+(row*40))
        .endrepeat
_bitmaplo:
        .repeat 25,row
        .byte <(soft80_bitmap+(row*40*8))
        .endrepeat
_bitmaphi:
        .repeat 25,row
        .byte >(soft80_bitmap+(row*40*8))
        .endrepeat

