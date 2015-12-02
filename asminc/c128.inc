;
; C128 generic definitions. Stolen from Elite128
;


; ---------------------------------------------------------------------------
; Zero page, Commodore stuff

TXTPTR          := $3D          ; Pointer into BASIC source code
TIME            := $A0          ; 60HZ clock
FNAM_LEN        := $B7          ; Length of filename
SECADR          := $B9          ; Secondary address
DEVNUM          := $BA          ; Device number
FNAM            := $BB          ; Address of filename
FNAM_BANK       := $C7          ; Bank for filename
KEY_COUNT       := $D0          ; Number of keys in input buffer
FKEY_COUNT      := $D1          ; Characters for function key
MODE            := $D7          ; 40/80 column mode flag
CURS_X          := $EC          ; Cursor column
CURS_Y          := $EB          ; Cursor row
SCREEN_PTR      := $E0          ; Pointer to current char in text screen
CRAM_PTR        := $E2          ; Pointer to current char in color RAM

CHARCOLOR       := $F1
RVS             := $F3          ; Reverse output flag
SCROLL          := $F8          ; Disable scrolling flag

BASIC_BUF       := $200         ; Location of command-line
BASIC_BUF_LEN   = 162           ; Maximum length of command-line

FETCH           := $2A2         ; Fetch subroutine in RAM
FETVEC          := $2AA         ; Vector patch location for FETCH
STASH           := $2AF         ; Stash routine in RAM
STAVEC          := $2B9         ; Vector patch location for STASH
IRQInd          := $2FD         ; JMP $0000 -- used as indirect IRQ vector
PALFLAG         := $A03         ; $FF=PAL, $00=NTSC
INIT_STATUS     := $A04         ; Flags: Reset/Restore initiation status
FKEY_LEN        := $1000        ; Function key lengths
FKEY_TEXT       := $100A        ; Function key texts

; ---------------------------------------------------------------------------
; Kernal routines

; Direct entries
CURS_SET        := $CD57
CURS_ON         := $CD6F
CURS_OFF        := $CD9F
CLRSCR          := $C142
KBDREAD         := $C006
NEWLINE         := $C363
PRINT           := $C322
NMIEXIT         := $FF33
INDFET          := $FF74

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

VDC_INDEX       := $D600
VDC_DATA        := $D601

; ---------------------------------------------------------------------------
; I/O: CIAs

CIA1            := $DC00
CIA1_PRA        := $DC00
CIA1_PRB        := $DC01
CIA1_DDRA       := $DC02
CIA1_DDRB       := $DC03
CIA1_TOD10      := $DC08
CIA1_TODSEC     := $DC09
CIA1_TODMIN     := $DC0A
CIA1_TODHR      := $DC0B
CIA1_ICR        := $DC0D
CIA1_CRA        := $DC0E
CIA1_CRB        := $DC0F

CIA2            := $DD00
CIA2_PRA        := $DD00
CIA2_PRB        := $DD01
CIA2_DDRA       := $DD02
CIA2_DDRB       := $DD03
CIA2_TOD10      := $DD08
CIA2_TODSEC     := $DD09
CIA2_TODMIN     := $DD0A
CIA2_TODHR      := $DD0B
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
