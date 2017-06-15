;-------------------------------------------------------------------------
; ANTIC Address Equates
;-------------------------------------------------------------------------

; Read Addresses

VCOUNT  = ANTIC + $0B         ;vertical line counter
PENH    = ANTIC + $0C         ;light pen horizontal position
PENV    = ANTIC + $0D         ;light pen vertical position
NMIST   = ANTIC + $0F         ;NMI interrupt status

; Write Addresses

DMACTL  = ANTIC + $00         ;DMA control
CHACTL  = ANTIC + $01         ;character control
DLISTL  = ANTIC + $02         ;low display list address
DLISTH  = ANTIC + $03         ;high display list address
HSCROL  = ANTIC + $04         ;horizontal scroll
VSCROL  = ANTIC + $05         ;vertical scroll
PMBASE  = ANTIC + $07         ;player-missile base address
CHBASE  = ANTIC + $09         ;character base address
WSYNC   = ANTIC + $0A         ;wait for HBLANK synchronization
NMIEN   = ANTIC + $0E         ;NMI enable
NMIRES  = ANTIC + $0F         ;NMI interrupt reset


;-------------------------------------------------------------------------
; Antic opcodes
;-------------------------------------------------------------------------

; usage example:
;
; ScreenDL:
; .byte DL_BLK8
; .byte DL_BLK8
; .byte DL_CHR40x8x1 | DL_LMS | DL_DLI
; .word ScreenAlignment
; .byte DL_BLK1 | DL_DLI
; .byte DL_MAP320x1x1 | DL_LMS
; .word Screen
;
; .repeat 99
; .byte DL_MAP320x1x1
; .endrepeat
; .byte DL_MAP320x1x1 | DL_LMS
; .word Screen + 40 * 100       ; 100 lines a 40 byte, 'Screen' has to be aligned correctly!
; .repeat 92
; .byte DL_MAP320x1x1
; .endrepeat
;
; .byte DL_JVB

; absolute instructions (non mode lines)

DL_JMP  = 1
DL_JVB  = 65

; DL_BLKn display n empty lines (just background)

DL_BLK1  = 0
DL_BLK2  = 16
DL_BLK3  = 32
DL_BLK4  = 48
DL_BLK5  = 64
DL_BLK6  = 80
DL_BLK7  = 96
DL_BLK8  = 112

; absolute instructions (mode lines)

DL_CHR40x8x1  = 2               ; monochrome, 40 character & 8 scanlines per mode line (GR. 0)
DL_CHR40x10x1 = 3               ; monochrome, 40 character & 10 scanlines per mode line
DL_CHR40x8x4  = 4               ; colour, 40 character & 8 scanlines per mode line (GR. 12)
DL_CHR40x16x4 = 5               ; colour, 40 character & 16 scanlines per mode line (GR. 13)
DL_CHR20x8x2  = 6               ; colour (duochrome per character), 20 character & 8 scanlines per mode line (GR. 1)
DL_CHR20x16x2 = 7               ; colour (duochrome per character), 20 character & 16 scanlines per mode line (GR. 2)

DL_MAP40x8x4  = 8               ; colour, 40 pixel & 8 scanlines per mode line (GR. 3)
DL_MAP80x4x2  = 9               ; 'duochrome', 80 pixel & 4 scanlines per mode line (GR.4) 
DL_MAP80x4x4  = 10              ; colour, 80 pixel & 4 scanlines per mode line (GR.5) 
DL_MAP160x2x2 = 11              ; 'duochrome', 160 pixel & 2 scanlines per mode line (GR.6) 
DL_MAP160x1x2 = 12              ; 'duochrome', 160 pixel & 1 scanline per mode line (GR.14) 
DL_MAP160x2x4 = 13              ; 4 colours, 160 pixel & 2 scanlines per mode line (GR.7) 
DL_MAP160x1x4 = 14              ; 4 colours, 160 pixel & 1 scanline per mode line (GR.15) 
DL_MAP320x1x1 = 15              ; monochrome, 320 pixel & 1 scanline per mode line (GR.8) 

; modifiers on mode lines...

DL_HSCROL = 16
DL_VSCROL = 32
DL_LMS    = 64

; general modifier...

DL_DLI    = 128
