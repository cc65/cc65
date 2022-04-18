;
; 2022-04-02, Karri Kaksonen
;
; The Atari 7800 has only 4k of RAM. So for a generic conio implementation
; the best alternative is to use indirect tile mapping with a character
; frame buffer
;

        .constructor    mono_initconio
        .include        "atari7800.inc"
        .include        "extzp.inc"
        .import         _mono_font
        .import         _get_tv
        .export         _mono_screen
        .export         _mono_zones
        .export         _mono_dll

        .bss
_mono_screen:
        .res    mono_charsperline * screenrows

;----------------------------------------------------------------------------
; Macros used to generate lists

.macro DLLentry offset, addr
        .byte   offset
        .byte   >addr
        .byte   <addr
.endmacro

.macro NullHeader offset, zero
        .byte   offset
        .byte   zero
.endmacro

.macro Header addr, palwidth, hpos
        .byte   <addr
        .byte   palwidth
        .byte   >addr
        .byte   hpos
.endmacro

.macro XHeader addr, flags, palwidth, hpos
        .byte   <addr
        .byte   flags
        .byte   >addr
        .byte   palwidth
        .byte   hpos
.endmacro

.macro TextZone row
        ; Text
        .byte   <(_mono_screen + 2 * row * charsperline)
        .byte   $60
        .byte   >(_mono_screen + 2 * row * charsperline)
        .byte   12
        .byte   0
        .byte   <(_mono_screen + 2 * row * charsperline + charsperline)
        .byte   $60
        .byte   >(_mono_screen + 2 * row * charsperline + charsperline)
        .byte   12
        .byte   80
        ; Cursor
        .byte   95
        .byte   0
        .byte   >_mono_font
        .byte   0
.endmacro

;-----------------------------------------------------------------------------
; The Atari 7800 has only 4k of RAM. So for a generic conio implementation
; the best alternative is to use indirect tile mapping with a character
; frame buffer
        .data

_mono_zones:
zone0:  TextZone 0
nh:     NullHeader 0, 0
zone1:  TextZone 1
        NullHeader 0, 0
zone2:  TextZone 2
        NullHeader 0, 0
zone3:  TextZone 3
        NullHeader 0, 0
zone4:  TextZone 4
        NullHeader 0, 0
zone5:  TextZone 5
        NullHeader 0, 0
zone6:  TextZone 6
        NullHeader 0, 0
zone7:  TextZone 7
        NullHeader 0, 0
zone8:  TextZone 8
        NullHeader 0, 0
zone9:  TextZone 9
        NullHeader 0, 0
zone10: TextZone 10
        NullHeader 0, 0
zone11: TextZone 11
        NullHeader 0, 0
zone12: TextZone 12
        NullHeader 0, 0
zone13: TextZone 13
        NullHeader 0, 0
zone14: TextZone 14
        NullHeader 0, 0
zone15: TextZone 15
        NullHeader 0, 0
zone16: TextZone 16
        NullHeader 0, 0
zone17: TextZone 17
        NullHeader 0, 0
zone18: TextZone 18
        NullHeader 0, 0
zone19: TextZone 19
        NullHeader 0, 0
zone20: TextZone 20
        NullHeader 0, 0
zone21: TextZone 21
        NullHeader 0, 0
zone22: TextZone 22
        NullHeader 0, 0
zone23: TextZone 23
        NullHeader 0, 0
zone24: TextZone 24
        NullHeader 0, 0
zone25: TextZone 25
        NullHeader 0, 0
zone26: TextZone 26
        NullHeader 0, 0
zone27: TextZone 27
        NullHeader 0, 0

_mono_dll:
PALscanlines:                           ; 25 lines
        DLLentry 15, nh
        DLLentry 8, nh

Topscanlines:                           ; 9 lines
        DLLentry 8, nh

Displaylines:
        DLLentry $80+7, zone0           ; NMI interrupt from end of prev zone
        DLLentry     7, zone1
        DLLentry     7, zone2
        DLLentry     7, zone3
        DLLentry     7, zone4
        DLLentry     7, zone5
        DLLentry     7, zone6
        DLLentry     7, zone7
        DLLentry     7, zone8
        DLLentry     7, zone9
        DLLentry     7, zone10
        DLLentry     7, zone11
        DLLentry     7, zone12
        DLLentry     7, zone13
        DLLentry     7, zone14
        DLLentry     7, zone15
        DLLentry     7, zone16
        DLLentry     7, zone17
        DLLentry     7, zone18
        DLLentry     7, zone19
        DLLentry     7, zone20
        DLLentry     7, zone21
        DLLentry     7, zone22
        DLLentry     7, zone23
        DLLentry     7, zone24
        DLLentry     7, zone25
        DLLentry     7, zone26
        DLLentry     7, zone27

Bottomscanlines:
        DLLentry $80+15, nh             ; NMI interrupt at end of display
        DLLentry 9, nh
        DLLentry 15, nh
        DLLentry 8, nh

;-----------------------------------------------------------------------------
; Set up the screen to 320a mode
;
        .segment        "ONCE"

CTRL_MODE160    .set    0
CTRL_MODEAC     .set    3
CTRL_KANGOFF    .set    0
CTRL_BCBLACK    .set    0
CTRL_CHAR1B     .set    0
CTRL_CHAR2B     .set    $10
CTRL_DMAON      .set    $40
CTRL_CKOFF      .set    0

;-----------------------------------------------------------------------------
; Initialize the conio display lists and zones
;
        .proc   mono_initconio

        jsr     _get_tv
        bne     pal
        lda     #<Topscanlines
        sta     DPPL
        lda     #>Topscanlines
        sta     DPPH
        jmp     vblankon
pal:    lda     #<PALscanlines
        sta     DPPL
        lda     #>PALscanlines
        sta     DPPH
vblankon:
        lda     MSTAT
        bmi     vblankon
vblankoff:
        lda     MSTAT
        bpl     vblankoff
        lda     #>_mono_font
        sta     CHBASE
        lda     #(CTRL_MODEAC | CTRL_KANGOFF | CTRL_BCBLACK | CTRL_CHAR1B | CTRL_DMAON | CTRL_CKOFF)
        sta     CTRL
        lda     #$00            ; Black background
        sta     BKGRND
        sta     CURS_X
        sta     CURS_Y
        lda     #$0f            ; White
        sta     P0C1
        rts

        .endproc

