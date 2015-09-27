;
; Low level stuff for screen output/console input
;

        .constructor    soft80_init, 24
        .destructor     soft80_shutdown

        .import         soft80_kclrscr
        .import         soft80_textcolor, soft80_bgcolor

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_init:
        lda     #$3b
        sta     VIC_CTRL1
        lda     #$00
        sta     CIA2_PRA
        lda     #$68
        sta     VIC_VIDEO_ADR
        lda     #$c8
        sta     VIC_CTRL2

; copy charset to RAM under I/O -> FIXME: generate at runtime
        sei
        lda     $01
        pha
        lda     #$34
        sta     $01

        lda     #>soft80_lo_charset0
        sta     @hi1+2
        lda     #>$d000
        sta     @hi2+2

        ldy     #8
@l2:
        ldx     #0
@l1:
@hi1:   lda     soft80_lo_charset0,x
@hi2:   sta     $d000,x
        inx
        bne     @l1
        inc     @hi1+2
        inc     @hi2+2
        dey
        bne     @l2

        pla
        sta     $01
        cli

        lda     646                     ; use current textcolor
        jsr     soft80_textcolor

        lda     VIC_BG_COLOR0           ; use current bgcolor
        and     #$0f
        jsr     soft80_bgcolor

        jmp     soft80_kclrscr

soft80_shutdown:
        lda     #$1b
        sta     VIC_CTRL1
        lda     #$03
        sta     CIA2_PRA
        lda     #$15
        sta     VIC_VIDEO_ADR
        rts

; FIXME: generate the charset at init time, and put it into RAM under I/O

        .include "soft80_charset.s"

