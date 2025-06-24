
; ---------------------------------------------------------------------------
; Zero page, Commodore stuff

TXTPTR          := $3C          ; Pointer into BASIC source code
STATUS          := $90          ; Kernal I/O completion status
FNAM_LEN        := $B7          ; Length of filename
SECADR          := $B9          ; Secondary address
DEVNUM          := $BA          ; Device number
FNAM            := $BB          ; Address of filename
FNAM_BANK       := $BE          ; Bank for filename
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
BASIC_BUF_LEN   = 161           ; Maximum length of command-line

FKEY_LEN        := $1000        ; Function key lengths
FKEY_TEXT       := $100A        ; Function key texts

PALFLAG         := $1103        ; $FF=PAL, $00=NTSC
INIT_STATUS     := $1104        ; Flags: Reset/Restore initiation status
TIME            := $110C        ; 60HZ clock

KBDREPEAT       := $111a
KBDREPEATRATE   := $111b
KBDREPEATDELAY  := $111c

; ---------------------------------------------------------------------------
; Vectors

IRQVec          := $0314
BRKVec          := $0316
NMIVec          := $0318

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
VIC_SPR_COLL    := $D01E
VIC_SPR_BG_COLL := $D01F

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


; ---------------------------------------------------------------------------
; I/O: FDC

FDC             := $D080

; ---------------------------------------------------------------------------
; I/O: SID

SID0            := $D400
SID0_S1Lo       := $D400
SID0_S1Hi       := $D401
SID0_PB1Lo      := $D402
SID0_PB1Hi      := $D403
SID0_Ctl1       := $D404
SID0_AD1        := $D405
SID0_SUR1       := $D406

SID0_S2Lo       := $D407
SID0_S2Hi       := $D408
SID0_PB2Lo      := $D409
SID0_PB2Hi      := $D40A
SID0_Ctl2       := $D40B
SID0_AD2        := $D40C
SID0_SUR2       := $D40D

SID0_S3Lo       := $D40E
SID0_S3Hi       := $D40F
SID0_PB3Lo      := $D410
SID0_PB3Hi      := $D411
SID0_Ctl3       := $D412
SID0_AD3        := $D413
SID0_SUR3       := $D414

SID0_FltLo      := $D415
SID0_FltHi      := $D416
SID0_FltCtl     := $D417
SID0_Amp        := $D418
SID0_ADConv1    := $D419
SID0_ADConv2    := $D41A
SID0_Noise      := $D41B
SID0_Read3      := $D41C

SID1            := $D420
SID1_S1Lo       := $D420
SID1_S1Hi       := $D421
SID1_PB1Lo      := $D422
SID1_PB1Hi      := $D423
SID1_Ctl1       := $D424
SID1_AD1        := $D425
SID1_SUR1       := $D426

SID1_S2Lo       := $D427
SID1_S2Hi       := $D428
SID1_PB2Lo      := $D429
SID1_PB2Hi      := $D42A
SID1_Ctl2       := $D42B
SID1_AD2        := $D42C
SID1_SUR2       := $D42D

SID1_S3Lo       := $D42E
SID1_S3Hi       := $D42F
SID1_PB3Lo      := $D430
SID1_PB3Hi      := $D431
SID1_Ctl3       := $D432
SID1_AD3        := $D433
SID1_SUR3       := $D434

SID1_FltLo      := $D435
SID1_FltHi      := $D436
SID1_FltCtl     := $D437
SID1_Amp        := $D438
SID1_ADConv1    := $D439
SID1_ADConv2    := $D43A
SID1_Noise      := $D43B
SID1_Read3      := $D43C

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
; I/O: DMA

DMA             := $D700


; ---------------------------------------------------------------------------
; Processor Port at $01

LORAM           = $01           ; Enable the basic rom
HIRAM           = $02           ; Enable the kernal rom
IOEN            = $04           ; Enable I/O
CASSDATA        = $08           ; Cassette data
CASSPLAY        = $10           ; Cassette: Play
CASSMOT         = $20           ; Cassette motor on
TP_FAST         = $80           ; Switch Rossmoeller TurboProcess to fast mode

RAMONLY         = $F8           ; (~(LORAM | HIRAM | IOEN)) & $FF

; temporary, to get conio working
XSIZE = 80
YSIZE = 50
