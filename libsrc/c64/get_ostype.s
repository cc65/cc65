;
; Stefan Haubenthal, Jul 10 2003
;
; C64DTV detection by
; Marco van den Heuvel, 2010-01-28
;
; unsigned char get_ostype(void)
;
; $AA US
; $64 PET-64
; $43 SX-64
; $03 EU_NEW
; $00 EU_OLD
; $FF C64DTV
;

        .export         _get_ostype

.proc   _get_ostype

        ldx     #0            ; Clear high byte

        ldy     #1
        sty     $d03f
        ldy     $d040
        cpy     $d000
        bne     @c64dtv
        inc     $d000
        cpy     $d040
        beq     @c64dtv

; Normal C64

        lda     $ff80
        rts

; C64 DTV

@c64dtv:
        stx     $d03f
        lda     #$ff
        rts
.endproc
