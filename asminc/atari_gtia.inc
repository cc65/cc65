;-------------------------------------------------------------------------
; CTIA/GTIA Address Equates
;-------------------------------------------------------------------------

; Read/Write Addresses

CONSOL  = GTIA + $1F         ;console switches and speaker control

; Read Addresses

M0PF    = GTIA + $00         ;missile 0 and playfield collision
M1PF    = GTIA + $01         ;missile 1 and playfield collision
M2PF    = GTIA + $02         ;missile 2 and playfield collision
M3PF    = GTIA + $03         ;missile 3 and playfield collision

P0PF    = GTIA + $04         ;player 0 and playfield collision
P1PF    = GTIA + $05         ;player 1 and playfield collision
P2PF    = GTIA + $06         ;player 2 and playfield collision
P3PF    = GTIA + $07         ;player 3 and playfield collision

M0PL    = GTIA + $08         ;missile 0 and player collision
M1PL    = GTIA + $09         ;missile 1 and player collision
M2PL    = GTIA + $0A         ;missile 2 and player collision
M3PL    = GTIA + $0B         ;missile 3 and player collision

P0PL    = GTIA + $0C         ;player 0 and player collision
P1PL    = GTIA + $0D         ;player 1 and player collision
P2PL    = GTIA + $0E         ;player 2 and player collision
P3PL    = GTIA + $0F         ;player 3 and player collision

TRIG0   = GTIA + $10         ;joystick trigger 0
TRIG1   = GTIA + $11         ;joystick trigger 1

TRIG2   = GTIA + $12         ;cartridge interlock
TRIG3   = GTIA + $13         ;ACMI module interlock

PAL     = GTIA + $14         ;##rev2## PAL/NTSC indicator

; Write Addresses

HPOSP0  = GTIA + $00         ;player 0 horizontal position
HPOSP1  = GTIA + $01         ;player 1 horizontal position
HPOSP2  = GTIA + $02         ;player 2 horizontal position
HPOSP3  = GTIA + $03         ;player 3 horizontal position

HPOSM0  = GTIA + $04         ;missile 0 horizontal position
HPOSM1  = GTIA + $05         ;missile 1 horizontal position
HPOSM2  = GTIA + $06         ;missile 2 horizontal position
HPOSM3  = GTIA + $07         ;missile 3 horizontal position

SIZEP0  = GTIA + $08         ;player 0 size
SIZEP1  = GTIA + $09         ;player 1 size
SIZEP2  = GTIA + $0A         ;player 2 size
SIZEP3  = GTIA + $0B         ;player 3 size

SIZEM   = GTIA + $0C         ;missile sizes

GRAFP0  = GTIA + $0D         ;player 0 graphics
GRAFP1  = GTIA + $0E         ;player 1 graphics
GRAFP2  = GTIA + $0F         ;player 2 graphics
GRAFP3  = GTIA + $10         ;player 3 graphics

GRAFM   = GTIA + $11         ;missile graphics

COLPM0  = GTIA + $12         ;player-missile 0 color/luminance
COLPM1  = GTIA + $13         ;player-missile 1 color/luminance
COLPM2  = GTIA + $14         ;player-missile 2 color/luminance
COLPM3  = GTIA + $15         ;player-missile 3 color/luminance

COLPF0  = GTIA + $16         ;playfield 0 color/luminance
COLPF1  = GTIA + $17         ;playfield 1 color/luminance
COLPF2  = GTIA + $18         ;playfield 2 color/luminance
COLPF3  = GTIA + $19         ;playfield 3 color/luminance

COLBK   = GTIA + $1A         ;background color/luminance

PRIOR   = GTIA + $1B         ;priority select
VDELAY  = GTIA + $1C         ;vertical delay
GRACTL  = GTIA + $1D         ;graphic control
HITCLR  = GTIA + $1E         ;collision clear


; Hue values

HUE_GREY        = 0
HUE_GOLD        = 1
HUE_GOLDORANGE  = 2
HUE_REDORANGE   = 3
HUE_ORANGE      = 4
HUE_MAGENTA     = 5
HUE_PURPLE      = 6
HUE_BLUE        = 7
HUE_BLUE2       = 8
HUE_CYAN        = 9
HUE_BLUEGREEN   = 10
HUE_BLUEGREEN2  = 11
HUE_GREEN       = 12
HUE_YELLOWGREEN = 13
HUE_YELLOW      = 14
HUE_YELLOWRED   = 15

; Color defines, similar to c64 colors (untested)

GTIA_COLOR_BLACK      = (HUE_GREY << 4)
GTIA_COLOR_WHITE      = (HUE_GREY << 4 | 7 << 1)
GTIA_COLOR_RED        = (HUE_REDORANGE << 4 | 1 << 1)
GTIA_COLOR_CYAN       = (HUE_CYAN << 4 | 3 << 1)
GTIA_COLOR_VIOLET     = (HUE_PURPLE << 4 | 4 << 1)
GTIA_COLOR_GREEN      = (HUE_GREEN << 4 | 2 << 1)
GTIA_COLOR_BLUE       = (HUE_BLUE << 4 | 2 << 1)
GTIA_COLOR_YELLOW     = (HUE_YELLOW << 4 | 7 << 1)
GTIA_COLOR_ORANGE     = (HUE_ORANGE << 4 | 5 << 1)
GTIA_COLOR_BROWN      = (HUE_YELLOW << 4 | 2 << 1)
GTIA_COLOR_LIGHTRED   = (HUE_REDORANGE << 4 | 6 << 1)
GTIA_COLOR_GRAY1      = (HUE_GREY << 4 | 2 << 1)
GTIA_COLOR_GRAY2      = (HUE_GREY << 4 | 3 << 1)
GTIA_COLOR_LIGHTGREEN = (HUE_GREEN << 4 | 6 << 1)
GTIA_COLOR_LIGHTBLUE  = (HUE_BLUE << 4 | 6 << 1)
GTIA_COLOR_GRAY3      = (HUE_GREY << 4 | 5 << 1)
