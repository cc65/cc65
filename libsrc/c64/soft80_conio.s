;
; Groepaz/Hitmen, 11.10.2015
;
; Low level init code for soft80 screen output/console input
;

        .constructor    soft80_init
        .destructor     soft80_shutdown

        .import         soft80_kclrscr, soft80_charset
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

