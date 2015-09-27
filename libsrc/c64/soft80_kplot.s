
        .export         soft80_kplot
        .export         soft80_plotinit

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_kplot:
        bcs     @getpos

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
        lsr     a
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

        ; FIXME: perhaps just include the respective tables directly?
soft80_plotinit:
        ; create screen-rows base tables (bitmap)
        lda     #<soft80_bitmap
        sta     SCREEN_PTR
        lda     #>soft80_bitmap
        sta     SCREEN_PTR+1

        ldx     #$00
l1:
        lda     SCREEN_PTR
        sta     _bitmaplo,x
        clc
        adc     #<(40*8)
        sta     SCREEN_PTR
        lda     SCREEN_PTR+1
        sta     _bitmaphi,x
        adc     #>(40*8)
        sta     SCREEN_PTR+1
        inx
        cpx     #25
        bne     l1

        ; create screen-rows base tables (colorram)

        lda     #<soft80_vram
        sta     CRAM_PTR
        lda     #>soft80_vram
        sta     CRAM_PTR+1

        ldx     #$00
l1b:
        lda     CRAM_PTR
        sta     _vramlo,x
        clc
        adc     #<(40)
        sta     CRAM_PTR
        lda     CRAM_PTR+1
        sta     _vramhi,x
        adc     #>(40)
        sta     CRAM_PTR+1
        inx
        cpx     #25
        bne     l1b

        rts

_bitmapxlo:
        .repeat 80,col1
        .byte <((col1/2)*8)
        .endrepeat

_bitmapxhi:
        .repeat 80,col
        .byte >((col/2)*8)
        .endrepeat

        .bss
_vramlo:
        .res 25
_vramhi:
        .res 25
_bitmaplo:
        .res 25
_bitmaphi:
        .res 25

