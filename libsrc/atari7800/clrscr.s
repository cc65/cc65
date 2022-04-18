
        .include        "atari7800.inc"

        .export _clrscr

        .import _screen
        .import pusha0,pushax
        .include "extzp.inc"

        .code

        .proc _clrscr

        lda     #<_screen
        ldx     #>_screen
        sta     ptr7800
        stx     ptr7800+1
        ldx     #screenrows
@L1:    ldy     #charsperline
        lda     #0
@L2:    sta     (ptr7800),y
        dey
        bne     @L2
        lda     ptr7800
        clc
        adc     #charsperline
        bcc     @L3
        inc     ptr7800+1
@L3:    dex
        bne     @L1
        rts

        .endproc

;-------------------------------------------------------------------------------
; force the init constructor to be imported

                .import initconio
conio_init      = initconio
