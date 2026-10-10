;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
; Optimizations & color fixing by Brandon Woodward
;

        .export paletteinit, colors

        .include "nes.inc"

;+---------+----------------------------------------------------------+
;|  $2000  | PPU Control Register #1 (W)                              |
;|         |                                                          |
;|         |    D7: Execute NMI on VBlank                             |
;|         |           0 = Disabled                                   |
;|         |           1 = Enabled                                    |
;|         |    D6: PPU Master/Slave Selection --+                    |
;|         |           0 = Master                +-- UNUSED           |
;|         |           1 = Slave               --+                    |
;|         |    D5: Sprite Size                                       |
;|         |           0 = 8x8                                        |
;|         |           1 = 8x16                                       |
;|         |    D4: Background Pattern Table Address                  |
;|         |           0 = $0000 (VRAM)                               |
;|         |           1 = $1000 (VRAM)                               |
;|         |    D3: Sprite Pattern Table Address                      |
;|         |           0 = $0000 (VRAM)                               |
;|         |           1 = $1000 (VRAM)                               |
;|         |    D2: PPU Address Increment                             |
;|         |           0 = Increment by 1                             |
;|         |           1 = Increment by 32                            |
;|         | D1-D0: Name Table Address                                |
;|         |         00 = $2000 (VRAM)                                |
;|         |         01 = $2400 (VRAM)                                |
;|         |         10 = $2800 (VRAM)                                |
;|         |         11 = $2C00 (VRAM)                                |
;+---------+----------------------------------------------------------+
;+---------+----------------------------------------------------------+
;|  $2001  | PPU Control Register #2 (W)                              |
;|         |                                                          |
;|         | D7-D5: Full Background Colour (when D0 == 1)             |
;|         |         000 = None  +------------+                       |
;|         |         001 = Green              | NOTE: Do not use more |
;|         |         010 = Blue               |       than one type   |
;|         |         100 = Red   +------------+                       |
;|         | D7-D5: Colour Intensity (when D0 == 0)                   |
;|         |         000 = None            +--+                       |
;|         |         001 = Intensify green    | NOTE: Do not use more |
;|         |         010 = Intensify blue     |       than one type   |
;|         |         100 = Intensify red   +--+                       |
;|         |    D4: Sprite Visibility                                 |
;|         |           0 = Sprites not displayed                      |
;|         |           1 = Sprites visible                            |
;|         |    D3: Background Visibility                             |
;|         |           0 = Background not displayed                   |
;|         |           1 = Background visible                         |
;|         |    D2: Sprite Clipping                                   |
;|         |           0 = Sprites invisible in left 8-pixel column   |
;|         |           1 = No clipping                                |
;|         |    D1: Background Clipping                               |
;|         |           0 = BG invisible in left 8-pixel column        |
;|         |           1 = No clipping                                |
;|         |    D0: Display Type                                      |
;|         |           0 = Colour display                             |
;|         |           1 = Monochrome display                         |
;+---------+----------------------------------------------------------+


;-----------------------------------------------------------------------------

.segment        "ONCE"

.proc   paletteinit

        lda     #>$3F00
        sta     PPU_VRAM_ADDR2
        ldx     #<$3F00
        stx     PPU_VRAM_ADDR2

; Copy the colors table
@l1:    ldx     #$F0
@l2:    ldy     colors-$F0,x
        sty     PPU_VRAM_IO
        inx
        bne     @l2
        asl     a
        bpl     @l1

        rts

.endproc

;-----------------------------------------------------------------------------

.rodata

colors: .byte $0f       ; 0 black
        .byte $20       ; 1 white
        .byte $06       ; 2 red
        .byte $2C       ; 3 cyan
        .byte $13       ; 4 violet
        .byte $1a       ; 5 green
        .byte $01       ; 6 blue
        .byte $38       ; 7 yellow
        .byte $17       ; 8 orange
        .byte $07       ; 9 brown
        .byte $26       ; a light red
        .byte $2d       ; b dark grey
        .byte $00       ; c middle grey
        .byte $2a       ; d light green
        .byte $21       ; e light blue
        .byte $3d       ; f light gray
