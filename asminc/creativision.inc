;*
;** VTech Creativision Definitions
;*

;** Screen
SCREEN_ROWS             = 24
SCREEN_COLS             = 32
SCREEN_PTR              := $3A
CURSOR_X                := $3C
CURSOR_Y                := $3D

;** VDP
VDP_DATA_R              := $2000
VDP_STATUS_R            := $2001
VDP_DATA_W              := $3000
VDP_CONTROL_W           := $3001

;** PIA
PIA0_DATA               := $1000
PIA0_STATUS             := $1001
PIA1_DATA               := $1002
PIA1_STATUS             := $1003

;** General
CH_VLINE                = 33
CH_HLINE                = 34
CH_ULCORNER             = 35
CH_URCORNER             = 36
CH_LLCORNER             = 37
CH_LRCORNER             = 38

;** I/O (Zero-page variables)
ZP_KEYBOARD             := $10
ZP_JOY0_DIR             := $11
ZP_JOY1_DIR             := $13
ZP_JOY0_BUTTONS         := $16
ZP_JOY1_BUTTONS         := $17

;** Joystick direction values (ZP_JOY0_DIR/ZP_JOY1_DIR)
JOY_N                   = $49
JOY_NNE                 = $48
JOY_NE                  = $47
JOY_ENE                 = $46
JOY_E                   = $45
JOY_ESE                 = $44
JOY_SE                  = $43
JOY_SSE                 = $42
JOY_S                   = $41
JOY_SSW                 = $40
JOY_SW                  = $4F
JOY_WSW                 = $4E
JOY_W                   = $4D
JOY_WNW                 = $4C
JOY_NW                  = $4B
JOY_NNW                 = $4A

;** BIOS routines
BIOS_NMI_RESET_ADDR     := $F808
BIOS_PLAY_TUNE1         := $FBD6
BIOS_PLAY_SONG          := $FBED
BIOS_PLAY_TUNE2         := $FCE6
BIOS_WRITE_VDP_REG      := $FE1F
BIOS_QUIET_PSG          := $FE54
BIOS_POKE_PSG           := $FE77
BIOS_IRQ1_ADDR          := $FF3F
BIOS_IRQ2_ADDR          := $FF52
