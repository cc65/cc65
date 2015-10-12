;
; Groepaz/Hitmen, 11.10.2015
;
; Low level init code for soft80 screen output/console input
;

        .constructor    soft80_init
        .destructor     soft80_shutdown

        .import         soft80_kclrscr, soft80_charset
        .export         soft80_internal_textcolor, soft80_internal_bgcolor
        .export         soft80_internal_cursorxlsb

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

        ; copy charset to RAM under I/O
        ; FIXME: move charset and this constructor into init segment
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
        sta     soft80_internal_textcolor

        lda     VIC_BG_COLOR0           ; use current bgcolor
        and     #$0f
        sta     soft80_internal_bgcolor
        asl     a
        asl     a
        asl     a
        asl     a
        ora     soft80_internal_textcolor
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

;-------------------------------------------------------------------------------
; FIXME: when the code is fixed to use the "init" segment, these variables must
;        be moved into a section other than .bss so they survive after the init
;        code has been run.

        .bss
soft80_internal_textcolor:
        .res 1
soft80_internal_bgcolor:
        .res 1
soft80_internal_cursorxlsb:
        .res 1

