;-------------------------------------------------------------------------------
;                                gamate.inc
;
;                   Gamate system specific definitions
;
; (w) 2015 Groepaz/Hitmen (groepaz@gmx.net)
;     based on technical reference by PeT (mess@utanet.at)
;-------------------------------------------------------------------------------

; look at gamate.h for comments, they are not duplicated here

AUDIO_BASE      = $4000

JOY_DATA        = $4400

JOY_DATA_UP     = $01
JOY_DATA_DOWN   = $02
JOY_DATA_LEFT   = $04
JOY_DATA_RIGHT  = $08
JOY_DATA_FIRE_A = $10
JOY_DATA_FIRE_B = $20
JOY_DATA_START  = $40
JOY_DATA_SELECT = $80

LCD_WIDTH       = 160
LCD_HEIGHT      = 152

LCD_BASE        = $5000
LCD_MODE        = $5001
LCD_XPOS        = $5002
LCD_YPOS        = $5003
LCD_X           = $5004
LCD_Y           = $5005
LCD_READ        = $5006
LCD_DATA        = $5007

LCD_MODE_INC_X  = $00
LCD_MODE_INC_Y  = $40
LCD_XPOS_PLANE1 = $00
LCD_XPOS_PLANE2 = $80

; constants for the conio implementation
charsperline    = (LCD_WIDTH / 8)
screenrows      = (LCD_HEIGHT / 8)

CH_HLINE        = 1
CH_VLINE        = 2

COLOR_WHITE     = 0
COLOR_GREY2     = 1
COLOR_GREY1     = 2
COLOR_BLACK     = 3

;-------------------------------------------------------------------------------
; bios zp usage:
;
; 06/07         address of character set bitplane 1
; 08/09         address of character set bitplane 2
; 0a            nmi $4800 echo
; 0b            irq counter
; 0c            nmi call cart nmi
; 0e/0f/10/11   big endian irq counter
; 15/16/17      ?/xpos/ypos
;
; e8            nmi reset to $ff
;

ZP_NMI_4800     = $0a
ZP_NMI_ENABLE   = $0c

