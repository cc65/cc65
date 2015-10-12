
;
; Groepaz/Hitmen, 12.10.2015
;
; lowlevel kplot function for the soft80 implementation
;

        .export         soft80_kplot
        .import         soft80_internal_cursorxlsb

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_kplot:
        bcs     @getpos

        stx     CURS_Y
        sty     CURS_X

        ; calc pointer to bitmap
        lda     soft80_bitmapylo,x
        clc
        adc     soft80_bitmapxlo,y
        sta     SCREEN_PTR
        lda     soft80_bitmapyhi,x
        adc     soft80_bitmapxhi,y
        sta     SCREEN_PTR+1

        tya
        and     #1
        sta     soft80_internal_cursorxlsb

        ; calc pointer to vram
        tya
        lsr     a

        clc
        adc     soft80_vramlo,x
        sta     CRAM_PTR
        lda     #0
        adc     soft80_vramhi,x
        sta     CRAM_PTR+1

@getpos:
        ldx     CURS_Y
        ldy     CURS_X
        rts

        ; FIXME: the following tables take up 260 bytes, perhaps move them
        ;        to 0xdc00... area in ram under i/o

        .rodata
soft80_bitmapxlo:
        .repeat 80,col
        .byte <((col/2)*8)
        .endrepeat
soft80_bitmapxhi:
        .repeat 80,col
        .byte >((col/2)*8)
        .endrepeat
soft80_vramlo:
        .repeat 25,row
        .byte <(soft80_vram+(row*40))
        .endrepeat
soft80_vramhi:
        .repeat 25,row
        .byte >(soft80_vram+(row*40))
        .endrepeat
soft80_bitmapylo:
        .repeat 25,row
        .byte <(soft80_bitmap+(row*40*8))
        .endrepeat
soft80_bitmapyhi:
        .repeat 25,row
        .byte >(soft80_bitmap+(row*40*8))
        .endrepeat

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import soft80_init
conio_init      = soft80_init
