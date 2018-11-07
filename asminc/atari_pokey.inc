;-------------------------------------------------------------------------
; POKEY Address Equates
;-------------------------------------------------------------------------

; Read Addresses

POT0    = POKEY + $00         ;potentiometer 0
POT1    = POKEY + $01         ;potentiometer 1
POT2    = POKEY + $02         ;potentiometer 2
POT3    = POKEY + $03         ;potentiometer 3
POT4    = POKEY + $04         ;potentiometer 4
POT5    = POKEY + $05         ;potentiometer 5
POT6    = POKEY + $06         ;potentiometer 6
POT7    = POKEY + $07         ;potentiometer 7

ALLPOT  = POKEY + $08         ;potentiometer port status
KBCODE  = POKEY + $09         ;keyboard code
RANDOM  = POKEY + $0A         ;random number generator
SERIN   = POKEY + $0D         ;serial port input
IRQST   = POKEY + $0E         ;IRQ interrupt status
SKSTAT  = POKEY + $0F         ;serial port and keyboard status

; Write Addresses

AUDF1   = POKEY + $00         ;channel 1 audio frequency
AUDC1   = POKEY + $01         ;channel 1 audio control

AUDF2   = POKEY + $02         ;channel 2 audio frequency
AUDC2   = POKEY + $03         ;channel 2 audio control

AUDF3   = POKEY + $04         ;channel 3 audio frequency
AUDC3   = POKEY + $05         ;channel 3 audio control

AUDF4   = POKEY + $06         ;channel 4 audio frequency
AUDC4   = POKEY + $07         ;channel 4 audio control

AUDCTL  = POKEY + $08         ;audio control
STIMER  = POKEY + $09         ;start timers
SKRES   = POKEY + $0A         ;reset SKSTAT status
POTGO   = POKEY + $0B         ;start potentiometer scan sequence
SEROUT  = POKEY + $0D         ;serial port output
IRQEN   = POKEY + $0E         ;IRQ interrupt enable
SKCTL   = POKEY + $0F         ;serial port and keyboard control


; KBCODE Values

KEY_NONE    = $FF

KEY_0       = $32
KEY_1       = $1F
KEY_2       = $1E
KEY_3       = $1A
KEY_4       = $18
KEY_5       = $1D
KEY_6       = $1B
KEY_7       = $33
KEY_8       = $35
KEY_9       = $30

KEY_A       = $3F
KEY_B       = $15
KEY_C       = $12
KEY_D       = $3A
KEY_E       = $2A
KEY_F       = $38
KEY_G       = $3D
KEY_H       = $39
KEY_I       = $0D
KEY_J       = $01
KEY_K       = $05
KEY_L       = $00
KEY_M       = $25
KEY_N       = $23
KEY_O       = $08
KEY_P       = $0A
KEY_Q       = $2F
KEY_R       = $28
KEY_S       = $3E
KEY_T       = $2D
KEY_U       = $0B
KEY_V       = $10
KEY_W       = $2E
KEY_X       = $16
KEY_Y       = $2B
KEY_Z       = $17

KEY_COMMA       = $20
KEY_PERIOD      = $22
KEY_SLASH       = $26
KEY_SEMICOLON   = $02
KEY_PLUS        = $06
KEY_ASTERISK    = $07
KEY_DASH        = $0E
KEY_EQUALS      = $0F
KEY_LESSTHAN    = $36
KEY_GREATERTHAN = $37

KEY_ESC     = $1C
KEY_TAB     = $2C
KEY_SPACE   = $21
KEY_RETURN  = $0C
KEY_DELETE  = $34
KEY_CAPS    = $3C
KEY_INVERSE = $27
KEY_HELP    = $11

KEY_F1      = $03
KEY_F2      = $04
KEY_F3      = $13
KEY_F4      = $14

KEY_SHIFT   = $40
KEY_CTRL    = $80

; Composed keys

KEY_EXCLAMATIONMARK = KEY_1 | KEY_SHIFT
KEY_QUOTE           = KEY_2 | KEY_SHIFT
KEY_HASH            = KEY_3 | KEY_SHIFT
KEY_DOLLAR          = KEY_4 | KEY_SHIFT
KEY_PERCENT         = KEY_5 | KEY_SHIFT
KEY_AMPERSAND       = KEY_6 | KEY_SHIFT
KEY_APOSTROPHE      = KEY_7 | KEY_SHIFT
KEY_AT              = KEY_8 | KEY_SHIFT
KEY_OPENINGPARAN    = KEY_9 | KEY_SHIFT
KEY_CLOSINGPARAN    = KEY_0 | KEY_SHIFT
KEY_UNDERLINE       = KEY_DASH | KEY_SHIFT
KEY_BAR             = KEY_EQUALS | KEY_SHIFT
KEY_COLON           = KEY_SEMICOLON | KEY_SHIFT
KEY_BACKSLASH       = KEY_PLUS | KEY_SHIFT
KEY_CIRCUMFLEX      = KEY_ASTERISK | KEY_SHIFT
KEY_OPENINGBRACKET  = KEY_COMMA | KEY_SHIFT
KEY_CLOSINGBRACKET  = KEY_PERIOD | KEY_SHIFT
KEY_QUESTIONMARK    = KEY_SLASH | KEY_SHIFT
KEY_CLEAR           = KEY_LESSTHAN | KEY_SHIFT
KEY_INSERT          = KEY_GREATERTHAN | KEY_SHIFT

KEY_UP              = KEY_UNDERLINE | KEY_CTRL
KEY_DOWN            = KEY_EQUALS | KEY_CTRL
KEY_LEFT            = KEY_PLUS | KEY_CTRL
KEY_RIGHT           = KEY_ASTERISK | KEY_CTRL
