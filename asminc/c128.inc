;
; C128 generic definitions. Stolen from Elite128
;


; ---------------------------------------------------------------------------
; Zero page, Commodore stuff

TXTPTR          := $3D          ; Pointer into BASIC source code
STATUS          := $90          ; Kernal I/O completion status
TIME            := $A0          ; 60HZ clock
FNAM_LEN        := $B7          ; Length of filename
SECADR          := $B9          ; Secondary address
DEVNUM          := $BA          ; Device number
FNAM            := $BB          ; Address of filename
FNAM_BANK       := $C7          ; Bank for filename
KEY_COUNT       := $D0          ; Number of keys in input buffer
FKEY_COUNT      := $D1          ; Characters for function key
MODE            := $D7          ; 40-/80-column mode (bit 7: 80 columns)
GRAPHM          := $D8          ; Graphics mode flags (bits 5-7)
CHARDIS         := $D9          ; Bit 2 shadow for location $01
CURS_X          := $EC          ; Cursor column
CURS_Y          := $EB          ; Cursor row
SCREEN_PTR      := $E0          ; Pointer to current char in text screen
CRAM_PTR        := $E2          ; Pointer to current char in color RAM

CHARCOLOR       := $F1
RVS             := $F3          ; Reverse output flag
SCROLL          := $F8          ; Disable scrolling flag

BASIC_BUF       := $0200        ; Location of command-line
BASIC_BUF_LEN   = 162           ; Maximum length of command-line

FETCH           := $02A2        ; Fetch subroutine in RAM
FETVEC          := $02AA        ; Vector patch location for FETCH
STASH           := $02AF        ; Stash routine in RAM
STAVEC          := $02B9        ; Vector patch location for STASH
IRQInd          := $02FD        ; JMP $0000 -- used as indirect IRQ vector
PALFLAG         := $0A03        ; $FF=PAL, $00=NTSC
INIT_STATUS     := $0A04        ; Flags: Reset/Restore initiation status
VM2             := $0A2D        ; VIC-IIe shadow for $D018 -- graphics mode
FKEY_LEN        := $1000        ; Function key lengths
FKEY_TEXT       := $100A        ; Function key texts

KBDREPEAT       := $028a
KBDREPEATRATE   := $028b
KBDREPEATDELAY  := $028c

; ---------------------------------------------------------------------------
; Vectors

IRQVec          := $0314
BRKVec          := $0316
NMIVec          := $0318
KeyStoreVec     := $033C

; ---------------------------------------------------------------------------
; I/O: VIC

VIC             := $D000
VIC_SPR0_X      := $D000
VIC_SPR0_Y      := $D001
VIC_SPR1_X      := $D002
VIC_SPR1_Y      := $D003
VIC_SPR2_X      := $D004
VIC_SPR2_Y      := $D005
VIC_SPR3_X      := $D006
VIC_SPR3_Y      := $D007
VIC_SPR4_X      := $D008
VIC_SPR4_Y      := $D009
VIC_SPR5_X      := $D00A
VIC_SPR5_Y      := $D00B
VIC_SPR6_X      := $D00C
VIC_SPR6_Y      := $D00D
VIC_SPR7_X      := $D00E
VIC_SPR7_Y      := $D00F
VIC_SPR_HI_X    := $D010
VIC_SPR_ENA     := $D015
VIC_SPR_EXP_Y   := $D017
VIC_SPR_EXP_X   := $D01D
VIC_SPR_MCOLOR  := $D01C
VIC_SPR_BG_PRIO := $D01B

VIC_SPR_MCOLOR0 := $D025
VIC_SPR_MCOLOR1 := $D026

VIC_SPR0_COLOR  := $D027
VIC_SPR1_COLOR  := $D028
VIC_SPR2_COLOR  := $D029
VIC_SPR3_COLOR  := $D02A
VIC_SPR4_COLOR  := $D02B
VIC_SPR5_COLOR  := $D02C
VIC_SPR6_COLOR  := $D02D
VIC_SPR7_COLOR  := $D02E

VIC_CTRL1       := $D011
VIC_CTRL2       := $D016

VIC_HLINE       := $D012

VIC_LPEN_X      := $D013
VIC_LPEN_Y      := $D014

VIC_VIDEO_ADR   := $D018

VIC_IRR         := $D019        ; Interrupt request register
VIC_IMR         := $D01A        ; Interrupt mask register

VIC_BORDERCOLOR := $D020
VIC_BG_COLOR0   := $D021
VIC_BG_COLOR1   := $D022
VIC_BG_COLOR2   := $D023
VIC_BG_COLOR3   := $D024

; 128 stuff:
VIC_KBD_128     := $D02F        ; Extended kbd bits (visible in 64 mode)
VIC_CLK_128     := $D030        ; Clock rate register (visible in 64 mode)


; ---------------------------------------------------------------------------
; I/O: SID

SID             := $D400
SID_S1Lo        := $D400
SID_S1Hi        := $D401
SID_PB1Lo       := $D402
SID_PB1Hi       := $D403
SID_Ctl1        := $D404
SID_AD1         := $D405
SID_SUR1        := $D406

SID_S2Lo        := $D407
SID_S2Hi        := $D408
SID_PB2Lo       := $D409
SID_PB2Hi       := $D40A
SID_Ctl2        := $D40B
SID_AD2         := $D40C
SID_SUR2        := $D40D

SID_S3Lo        := $D40E
SID_S3Hi        := $D40F
SID_PB3Lo       := $D410
SID_PB3Hi       := $D411
SID_Ctl3        := $D412
SID_AD3         := $D413
SID_SUR3        := $D414

SID_FltLo       := $D415
SID_FltHi       := $D416
SID_FltCtl      := $D417
SID_Amp         := $D418
SID_ADConv1     := $D419
SID_ADConv2     := $D41A
SID_Noise       := $D41B
SID_Read3       := $D41C

; ---------------------------------------------------------------------------
; I/O: VDC (128 only)

VDC_INDEX       := $D600        ; register address port
VDC_DATA        := $D601        ; data port

; Registers
VDC_DATA_HI     = 18            ; video RAM address (big endian)
VDC_DATA_LO     = 19
VDC_CSET        = 28
VDC_RAM_RW      = 31            ; RAM port

; ---------------------------------------------------------------------------
; I/O: Complex Interface Adapters

CIA1            := $DC00
CIA1_PRA        := $DC00        ; Port A
CIA1_PRB        := $DC01        ; Port B
CIA1_DDRA       := $DC02        ; Data direction register for port A
CIA1_DDRB       := $DC03        ; Data direction register for port B
CIA1_TA         := $DC04        ; 16-bit timer A
CIA1_TB         := $DC06        ; 16-bit timer B
CIA1_TOD10      := $DC08        ; Time-of-day tenths of a second
CIA1_TODSEC     := $DC09        ; Time-of-day seconds
CIA1_TODMIN     := $DC0A        ; Time-of-day minutes
CIA1_TODHR      := $DC0B        ; Time-of-day hours
CIA1_SDR        := $DC0C        ; Serial data register
CIA1_ICR        := $DC0D        ; Interrupt control register
CIA1_CRA        := $DC0E        ; Control register for timer A
CIA1_CRB        := $DC0F        ; Control register for timer B

CIA2            := $DD00
CIA2_PRA        := $DD00
CIA2_PRB        := $DD01
CIA2_DDRA       := $DD02
CIA2_DDRB       := $DD03
CIA2_TA         := $DD04
CIA2_TB         := $DD06
CIA2_TOD10      := $DD08
CIA2_TODSEC     := $DD09
CIA2_TODMIN     := $DD0A
CIA2_TODHR      := $DD0B
CIA2_SDR        := $DD0C
CIA2_ICR        := $DD0D
CIA2_CRA        := $DD0E
CIA2_CRB        := $DD0F

; ---------------------------------------------------------------------------
; I/O: MMU

MMU_CR          := $FF00
MMU_CFG_CC65    := %00001110    ; Bank 0 with kernal ROM
MMU_CFG_RAM0    := %00111111    ; Bank 0 full RAM
MMU_CFG_RAM1    := %01111111    ; Bank 1 full RAM
MMU_CFG_RAM2    := %10111111    ; Bank 2 full RAM
MMU_CFG_RAM3    := %11111111    ; Bank 3 full RAM
MMU_CFG_IFROM   := %01010111    ; Bank 1 with Internal Function RAM/ROM
MMU_CFG_EFROM   := %01101011    ; Bank 1 with External Function RAM/ROM

; ---------------------------------------------------------------------------
; Super CPU

SCPU_VIC_Bank1  := $D075
SCPU_Slow       := $D07A
SCPU_Fast       := $D07B
SCPU_EnableRegs := $D07E
SCPU_DisableRegs:= $D07F
SCPU_Detect     := $D0BC
