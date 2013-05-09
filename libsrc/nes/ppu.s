;
; Written by Groepaz/Hitmen <groepaz@gmx.net>
; Cleanup by Ullrich von Bassewitz <uz@cc65.org>
;

        .export ppuinit
        .export paletteinit, paldata

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

.proc   ppuinit

        lda     #%10101000
        sta     PPU_CTRL1

        lda     #%00011110
        sta     PPU_CTRL2

; Wait for vblank

@wait:  lda     PPU_STATUS
        bpl     @wait

; reset scrolling

        lda     #0
        sta     PPU_VRAM_ADDR1
        sta     PPU_VRAM_ADDR1

; Make all sprites invisible

        lda     #$00
        ldy     #$f0
        sta     PPU_SPR_ADDR
        ldx     #$40
@loop:  sty     PPU_SPR_IO
        sta     PPU_SPR_IO
        sta     PPU_SPR_IO
        sty     PPU_SPR_IO
        dex
        bne     @loop

        rts

.endproc


;-----------------------------------------------------------------------------

.proc   paletteinit

; Wait for v-blank
@wait:  lda     PPU_STATUS
        bpl     @wait

        lda     #$3F
        sta     PPU_VRAM_ADDR2
        lda     #$00
        sta     PPU_VRAM_ADDR2

        ldx     #0
@loop:  lda     paldata,x
        sta     PPU_VRAM_IO
        inx
        cpx     #(16*2)
        bne     @loop

        rts
                     
.endproc

;-----------------------------------------------------------------------------

.rodata

paldata:
        .repeat 2
        .byte   $0f     ; 0 black
        .byte   $14     ; 4 violett
        .byte   $3b     ; 3 cyan
        .byte   $3d     ; 1 white

        .byte   $38     ; 7 yellow
        .byte   $2d     ; b dark grey
        .byte   $22     ; e light blue
        .byte   $04     ; 2 red

        .byte   $18     ; 8 orange
        .byte   $08     ; 9 brown
        .byte   $35     ; a light red
        .byte   $01     ; 6 blue

        .byte   $10     ; c middle grey
        .byte   $2b     ; d light green
        .byte   $3d     ; f light gray
        .byte   $1a     ; 5 green
        .endrepeat
