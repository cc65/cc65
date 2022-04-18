
        .include        "atari7800.inc"

        .export _mono_clrscr

        .import _momo_screen
        .import pusha0,pushax
        .include "extzp.inc"

        .code

        .proc _mono_clrscr

        lda     #<_momo_screen
        ldx     #>_momo_screen
        sta     ptr7800
        stx     ptr7800+1
        ldx     #screenrows
@L1:    ldy     #mono_charsperline
        lda     #0
@L2:    sta     (ptr7800),y
        dey
        bne     @L2
        lda     ptr7800
        clc
        adc     #mono_charsperline
        bcc     @L3
        inc     ptr7800+1
@L3:    dex
        bne     @L1
        rts

        .endproc

;-------------------------------------------------------------------------------
; force the init constructor to be imported

                .import mono_initconio
conio_init      = mono_initconio
