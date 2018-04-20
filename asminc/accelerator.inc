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
; C128 in C64 mode

C128_VICIIE_CLK := $D030

