
        .export         soft80_kclrscr
        .import         soft80_kplot
        .import         __bgcolor

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_kclrscr:

        lda     #$ff

        ldx     #$00
lp1:
        .repeat $1f,page
        sta     soft80_bitmap+(page*$100),x
        .endrepeat
        sta     soft80_bitmap+$1e40,x
        inx
        bne     lp1

        sei
        ldy     $01
        lda     #$34
        sta     $01

        lda     CHARCOLOR
        and     #$f0
        ora     __bgcolor

        ;ldx     #$00
lp2:
        sta     soft80_vram,x
        sta     soft80_vram+$100,x
        sta     soft80_vram+$200,x
        sta     soft80_vram+$2e8,x
        inx
        bne     lp2

        inc     $01

        lda     __bgcolor
        ;ldx     #$00
lp3:
        sta     soft80_colram,x
        sta     soft80_colram+$100,x
        sta     soft80_colram+$200,x
        sta     soft80_colram+$2e8,x
        inx
        bne     lp3


        sty     $01
        cli

        ldx     #0
        ldy     #0
        clc
        jmp     soft80_kplot




