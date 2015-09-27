
        .export         soft80_kclrscr
        .import         soft80_kplot
        .import         __bgcolor        ; FIX/CHECK

        .include        "c64.inc"
        .include        "soft80.inc"

soft80_kclrscr:

        lda     #$ff

        ldx     #$00
lp1:
        .repeat $20,page
        sta     soft80_bitmap+(page*$100),x
        .endrepeat
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
        .repeat $4,page
        sta     soft80_vram+(page*$100),x
        .endrepeat
        inx
        bne     lp2

        inc     $01

        lda     __bgcolor
        ;ldx     #$00
lp3:
        .repeat $4,page
        sta     soft80_colram+(page*$100),x
        .endrepeat
        inx
        bne     lp3


        sty     $01
        cli

        ldx     #0
        ldy     #0
        clc
        jmp     soft80_kplot




