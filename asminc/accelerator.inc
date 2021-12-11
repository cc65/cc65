;
; Accelerator definitions.
;

; ---------------------------------------------------------------------------
; Speed definitions for all accelerator, to be used as input for the 'set'
; functions.


SPEED_SLOW = $00
SPEED_FAST = $FF

SPEED_1X    = SPEED_SLOW
SPEED_2X    =  2 - 1
SPEED_3X    =  3 - 1
SPEED_4X    =  4 - 1
SPEED_5X    =  5 - 1
SPEED_6X    =  6 - 1
SPEED_7X    =  7 - 1
SPEED_8X    =  8 - 1
SPEED_10X   = 10 - 1
SPEED_12X   = 12 - 1
SPEED_16X   = 16 - 1
SPEED_20X   = 20 - 1


; ---------------------------------------------------------------------------
; C64/C128 Super CPU cartridge

SuperCPU_Slow       := $D07A
SuperCPU_Fast       := $D07B
SuperCPU_Speed_Mode := $D0B8
SuperCPU_Detect     := $D0BC


; ---------------------------------------------------------------------------
; C64DTV

C64DTV_Extended_Regs := $D03F

C64DTV_Slow = $00
C64DTV_Fast = $03


; ---------------------------------------------------------------------------
; C128 native and C128 in C64 mode

C128_VICIIE_CLK := $D030


; ---------------------------------------------------------------------------
; C64 Chameleon cartridge

CHAMELEON_CFGTUR := $D0F3
CHAMELEON_CFGENA := $D0FE

CHAMELEON_ENABLE_REGS  = $2A
CHAMELEON_DISABLE_REGS = $FF

CHAMELEON_CFGTUR_LIMIT_1MHZ = %00001100
CHAMELEON_CFGTUR_LIMIT_NONE = %10000000


; ---------------------------------------------------------------------------
; C65/C64DX in C64 mode

C65_VICIII_KEY    := $D02F
C65_VICIII_CTRL_B := $D031

C65_VICIII_UNLOCK_1 = $A5
C65_VICIII_UNLOCK_2 = $96


; ---------------------------------------------------------------------------
; C64 Turbo Master cartridge

TURBOMASTER_DETECT    := $BF53

TURBOMASTER_SPEED_REG := $00

