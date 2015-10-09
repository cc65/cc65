;
; Low level stuff for screen output/console input
;

        .constructor    soft80_init
        .destructor     soft80_shutdown

        .import         soft80_kclrscr
        .import         __textcolor, __bgcolor

        .importzp       ptr1, ptr2, ptr3

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

        lda     #>soft80_charset
        sta     ptr1+1
        lda     #<soft80_charset
        sta     ptr1
        lda     #>soft80_lo_charset
        sta     ptr2+1
        lda     #<soft80_lo_charset
        sta     ptr2
        lda     #>soft80_hi_charset
        sta     ptr3+1
        lda     #<soft80_hi_charset
        sta     ptr3

        ldx     #4
@l2:
        ldy     #0
@l1:
        lda     (ptr1),y
        sta     (ptr2),y
        asl     a
        asl     a
        asl     a
        asl     a
        sta     (ptr3),y
        iny
        bne     @l1
        inc     ptr1+1
        inc     ptr2+1
        inc     ptr3+1
        dex
        bne     @l2

        pla
        sta     $01
        cli

        lda     646                     ; use current textcolor
        and     #$0f
        sta     __textcolor

        lda     VIC_BG_COLOR0           ; use current bgcolor
        and     #$0f
        sta     __bgcolor
        asl     a
        asl     a
        asl     a
        asl     a
        ora     __textcolor
        sta     CHARCOLOR

        jmp     soft80_kclrscr

soft80_shutdown:
        lda     #$1b
        sta     VIC_CTRL1
        lda     #$03
        sta     CIA2_PRA
        lda     #$15
        sta     VIC_VIDEO_ADR
        rts

        .rodata
soft80_charset:
        .byte $0f,$03,$0f,$00,$0f,$07,$05,$0e
        .byte $0f,$05,$0e,$0b,$0f,$0b,$0f,$0f
        .byte $0f,$0b,$0f,$0b,$07,$07,$0e,$00
        .byte $00,$0f,$0e,$0f,$0c,$0b,$03,$03
        .byte $0f,$0b,$05,$05,$0b,$05,$0b,$0b
        .byte $0d,$07,$0f,$0f,$0f,$0f,$0f,$0d
        .byte $0b,$0b,$0b,$0b,$05,$01,$0b,$01
        .byte $0b,$0b,$0f,$0f,$0d,$0f,$07,$0b
        .byte $0b,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$09,$07,$03,$0b,$0f
        .byte $0f,$0b,$03,$0b,$03,$01,$01,$0b
        .byte $05,$01,$09,$05,$07,$05,$05,$0b
        .byte $03,$0b,$03,$0b,$01,$05,$05,$05
        .byte $05,$05,$01,$0b,$07,$0b,$0f,$0a

        .byte $0f,$03,$0f,$0f,$0f,$07,$05,$0e
        .byte $0f,$0a,$0e,$0b,$0f,$0b,$0f,$0f
        .byte $0f,$0b,$0f,$0b,$07,$07,$0e,$00
        .byte $00,$0f,$0e,$0f,$0c,$0b,$03,$03
        .byte $0f,$0b,$05,$05,$09,$05,$05,$0b
        .byte $0b,$0b,$05,$0b,$0f,$0f,$0f,$0d
        .byte $05,$0b,$05,$05,$05,$07,$05,$05
        .byte $05,$05,$0f,$0f,$0b,$0f,$0b,$05
        .byte $05,$0f,$07,$0f,$0d,$0f,$09,$0f
        .byte $07,$0b,$0d,$07,$03,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0b,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0b,$07,$0b,$0b,$0b
        .byte $0f,$0b,$05,$05,$05,$07,$07,$05
        .byte $05,$0b,$0d,$05,$07,$01,$01,$05
        .byte $05,$05,$05,$05,$0b,$05,$05,$05
        .byte $05,$05,$0d,$0b,$07,$0b,$0f,$0a

        .byte $0f,$03,$0f,$0f,$0f,$07,$0a,$0e
        .byte $0f,$05,$0e,$0b,$0f,$0b,$0f,$0f
        .byte $0f,$0b,$0f,$0b,$07,$07,$0e,$0f
        .byte $00,$0f,$0d,$0f,$0c,$0b,$03,$03
        .byte $0f,$0b,$05,$00,$07,$0d,$0b,$07
        .byte $0b,$0b,$0b,$0b,$0f,$0f,$0f,$0b
        .byte $01,$03,$0d,$0d,$05,$03,$07,$0d
        .byte $05,$05,$0b,$0b,$0b,$08,$0b,$0d
        .byte $01,$0b,$07,$09,$0d,$0b,$0b,$09
        .byte $07,$0f,$0f,$07,$0b,$05,$03,$0b
        .byte $03,$09,$03,$09,$01,$05,$05,$05
        .byte $05,$05,$01,$0b,$0b,$0b,$05,$0b
        .byte $0f,$05,$05,$07,$05,$07,$07,$07
        .byte $05,$0b,$0d,$03,$07,$01,$01,$05
        .byte $05,$05,$05,$07,$0b,$05,$05,$05
        .byte $0b,$05,$0b,$0b,$0b,$0b,$0a,$05

        .byte $09,$03,$0f,$0f,$0f,$07,$0a,$0e
        .byte $0f,$0a,$0e,$08,$0f,$08,$03,$0f
        .byte $08,$00,$00,$03,$07,$07,$0e,$0f
        .byte $0f,$0f,$05,$0f,$0c,$03,$03,$03
        .byte $0f,$0b,$0f,$05,$0b,$0b,$0b,$0f
        .byte $0b,$0b,$01,$01,$0f,$01,$0f,$0b
        .byte $05,$0b,$0b,$0b,$01,$0d,$03,$0b
        .byte $0b,$09,$0f,$0f,$07,$0f,$0d,$0b
        .byte $01,$0d,$03,$07,$09,$05,$01,$05
        .byte $03,$03,$0d,$05,$0b,$01,$05,$05
        .byte $05,$05,$05,$07,$0b,$05,$05,$05
        .byte $05,$05,$0d,$0b,$0b,$0b,$05,$00
        .byte $00,$01,$03,$07,$05,$03,$03,$01
        .byte $01,$0b,$0d,$03,$07,$05,$01,$05
        .byte $03,$05,$03,$0b,$0b,$05,$05,$01
        .byte $0b,$0b,$0b,$00,$0b,$0b,$05,$05

        .byte $09,$03,$00,$0f,$0f,$07,$05,$0e
        .byte $05,$05,$0e,$08,$0c,$08,$03,$0f
        .byte $08,$00,$00,$03,$07,$07,$0e,$0f
        .byte $0f,$0f,$03,$03,$0f,$03,$0f,$0c
        .byte $0f,$0f,$0f,$00,$0d,$07,$04,$0f
        .byte $0b,$0b,$0b,$0b,$0f,$0f,$0f,$0b
        .byte $05,$0b,$07,$0d,$0d,$0d,$05,$0b
        .byte $05,$0d,$0f,$0f,$0b,$08,$0b,$0b
        .byte $07,$09,$05,$07,$05,$01,$0b,$05
        .byte $05,$0b,$0d,$03,$0b,$01,$05,$05
        .byte $05,$05,$07,$0b,$0b,$05,$05,$01
        .byte $0b,$05,$0b,$0b,$0b,$0b,$0f,$00
        .byte $00,$05,$05,$07,$05,$07,$07,$05
        .byte $05,$0b,$0d,$03,$07,$05,$01,$05
        .byte $07,$05,$03,$0d,$0b,$05,$05,$01
        .byte $0b,$0b,$0b,$00,$07,$0b,$05,$0a

        .byte $0f,$03,$00,$0f,$0f,$07,$05,$0e
        .byte $05,$0a,$0e,$0b,$0c,$0f,$0b,$0f
        .byte $0b,$0f,$0b,$0b,$07,$07,$0e,$0f
        .byte $0f,$00,$03,$03,$0f,$0f,$0f,$0c
        .byte $0f,$0f,$0f,$05,$03,$05,$05,$0f
        .byte $0b,$0b,$05,$0b,$0b,$0f,$0b,$07
        .byte $05,$0b,$07,$05,$0d,$05,$05,$0b
        .byte $05,$05,$0b,$0b,$0b,$0f,$0b,$0f
        .byte $05,$05,$05,$07,$05,$07,$0b,$09
        .byte $05,$0b,$0d,$05,$0b,$05,$05,$05
        .byte $03,$09,$07,$0d,$0b,$05,$0b,$01
        .byte $05,$09,$07,$0b,$0d,$0b,$0f,$0b
        .byte $0f,$05,$05,$05,$05,$07,$07,$05
        .byte $05,$0b,$05,$05,$07,$05,$05,$05
        .byte $07,$0b,$05,$05,$0b,$05,$0b,$05
        .byte $05,$0b,$07,$0b,$07,$0b,$05,$0a

        .byte $0f,$03,$00,$0f,$0f,$07,$0a,$0e
        .byte $0a,$05,$0e,$0b,$0c,$0f,$0b,$00
        .byte $0b,$0f,$0b,$0b,$07,$07,$0e,$0f
        .byte $0f,$00,$07,$03,$0f,$0f,$0f,$0c
        .byte $0f,$0b,$0f,$05,$0b,$05,$08,$0f
        .byte $0d,$07,$0f,$0f,$0b,$0f,$0b,$07
        .byte $0b,$01,$01,$0b,$0d,$0b,$0b,$0b
        .byte $0b,$0b,$0f,$0b,$0d,$0f,$07,$0b
        .byte $0b,$09,$03,$09,$09,$09,$0b,$0d
        .byte $05,$01,$0d,$05,$01,$05,$05,$0b
        .byte $07,$0d,$07,$03,$0d,$09,$0b,$05
        .byte $05,$0d,$01,$09,$0d,$03,$0f,$0b
        .byte $0f,$05,$03,$0b,$03,$01,$07,$0b
        .byte $05,$01,$0b,$05,$01,$05,$05,$0b
        .byte $07,$0d,$05,$0b,$0b,$0b,$0b,$05
        .byte $05,$0b,$01,$0b,$0b,$0b,$05,$05

        .byte $0f,$03,$00,$0f,$00,$07,$0a,$0e
        .byte $0a,$0a,$0e,$0b,$0c,$0f,$0b,$00
        .byte $0b,$0f,$0b,$0b,$07,$07,$0e,$0f
        .byte $0f,$00,$0f,$03,$0f,$0f,$0f,$0c
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$07,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$07,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$03
        .byte $0f,$0f,$03,$0f,$0f,$0f,$0f,$0f
        .byte $07,$0d,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$03,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0f,$0f,$0f,$0f,$0f
        .byte $0f,$0f,$0f,$0b,$0b,$0b,$0f,$05
