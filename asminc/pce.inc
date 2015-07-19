;
; PCE definitions. By Groepaz/Hitmem.
;

;; FIXME: optimize zeropage usage

CURS_X      = $30
CURS_Y      = $31
SCREEN_PTR  = $32       ;2
CRAM_PTR    = $34       ;2
CHARCOLOR   = $36
RVS         = $37
BGCOLOR     = $38
tickcount   = $39       ;4

screenrows = (224/8)
charsperline = 61

CH_HLINE = 1
CH_VLINE = 2

; huc6270 - Video Display Controller (vdc)

VDC_MAWR = 0 ; Memory Address Write Register
VDC_MARR = 1 ; Memory Address Read Register
VDC_VWR  = 2 ; VRAM Write Register
VDC_VRR  = 3 ; VRAM Read Register
VDC_CR   = 4 ; Control Register
VDC_RCR  = 5 ; Raster Counter Register
VDC_BXR  = 6 ; Background X-Scroll Register
VDC_BYR  = 7 ; Background Y-Scroll Register
VDC_MWR  = 8 ; Memory-access Width Register
VDC_HSR  = 9 ; Horizontal Sync Register (?)
VDC_HDR  =10 ; Horizontal Display Register (?)
VDC_VPR  =11 ; (unknown)
VDC_VDW  =12 ; (unknown use)
VDC_VCR  =13 ; (unknown use)
VDC_DCR  =14 ; (DMA) Control Register
VDC_SOUR =15 ; (DMA) Source Register
VDC_DESR =16 ; (DMA) Destination Register
VDC_LENR =17 ; (DMA) Length Register
VDC_SATB =18 ; Sprite Attribute Table

; VDC port
; Note: absolute addressing mode must be used when writing to this port

VDC_CTRL        = $0000
VDC_DATA_LO     = $0002
VDC_DATA_HI     = $0003

; huc6260 - Video Color Encoder (vce)

; The DAC has a palette of 512 colours.
; bitmap of the palette data is this: 0000000gggrrrbbb.
; You can read and write the DAC-registers.

VCE         = $0400 ; base

VCE_CTRL    = $0400 ; write$00 to reset
VCE_ADDR_LO = $0402 ; LSB of byte offset into palette
VCE_ADDR_HI = $0403 ; MSB of byte offset into palette
VCE_DATA_LO = $0404 ; LSB of 16-bit palette data
VCE_DATA_HI = $0405 ; MSB of 16-bit palette data

; programmable sound generator (PSG)

PSG             = $0800 ; base

; timer

TIMER           = $0c00 ; base

TIMER_COUNT     = $0c00
TIMER_CTRL      = $0c01

JOY_CTRL        = $1000

IRQ_MASK        = $1402
IRQ_STATUS      = $1403

CDR_MEM_DISABLE = $1803
CDR_MEM_ENABLE  = $1807

; Write VDC register
.macro VREG arg1,arg2
        st0     #arg1
        st1     #<(arg2)
        st2     #>(arg2)
.endmacro
