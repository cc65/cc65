
;
; Groepaz/Hitmen, 19.10.2015
;
; lowlevel kplot function for the monochrome soft80 implementation
;

        .export         soft80mono_kplot
        .import         soft80mono_internal_cursorxlsb

        .include        "c64.inc"
        .include        "soft80.inc"

soft80mono_kplot:
        bcs     @getpos

        stx     CURS_Y
        sty     CURS_X

        sei
        lda     $01
        pha
        lda     #$34                            ; enable RAM under I/O
        sta     $01

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
        sta     soft80mono_internal_cursorxlsb

        pla
        sta     $01
        cli

@getpos:
        ldx     CURS_Y
        ldy     CURS_X
        rts

;-------------------------------------------------------------------------------
; force the init constructor to be imported

        .import soft80mono_init
conio_init      = soft80mono_init
