;
; PCE definitions. By Groepaz/Hitmen.
;

; FIXME: Screen dimensions can change according to the selected video mode.
screenrows      = (224/8)
charsperline    = 61

CH_HLINE        = 1
CH_VLINE        = 2

; HuC6270 -- Video Display Controller (VDC)

VDC_MAWR        = 0             ; Memory Address Write Register
VDC_MARR        = 1             ; Memory Address Read Register
VDC_VWR         = 2             ; VRAM Write Register
VDC_VRR         = 2             ; VRAM Read Register
VDC_UNK03       = 3             ; (unknown)
VDC_UNK04       = 4             ; (unknown)
VDC_CR          = 5             ; Control Register
VDC_RCR         = 6             ; Raster Counter Register
VDC_BXR         = 7             ; Background X-Scroll Register
VDC_BYR         = 8             ; Background Y-Scroll Register
VDC_MWR         = 9             ; Memory-access Width Register
VDC_HSR         = 10            ; Horizontal Sync Register
VDC_HDR         = 11            ; Horizontal Display Register
VDC_VSR         = 12            ; Vertical sync Register
VDC_VDR         = 13            ; Vertical Display register
VDC_VCR         = 14            ; Vertical display END position register
VDC_DCR         = 15            ; (DMA) Control Register
VDC_SOUR        = 16            ; (DMA) Source Register
VDC_DESR        = 17            ; (DMA) Destination Register
VDC_LENR        = 18            ; (DMA) Length Register
VDC_SATB        = 19            ; Sprite Attribute Table

; VDC port
; Note:  The zero-page addressing mode is redirected to page $20.
; We avoid it by using mirror locations that are outside of the zero page.

VDC_CTRL        := $0200
VDC_DATA_LO     := $0202
VDC_DATA_HI     := $0203

; HuC6260 -- Video Color Encoder (VCE)

; The DAC has a palette of 512 colours.
; The bitmap of that data is 0000000gggrrrbbb (Green, Red, Blue).
; You can read and write the DAC registers.

VCE             := $0400        ; base

VCE_CTRL        := $0400        ; write $00 to reset
VCE_ADDR_LO     := $0402        ; LSB of byte offset into palette
VCE_ADDR_HI     := $0403        ; MSB of byte offset into palette
VCE_DATA_LO     := $0404        ; LSB of 16-bit palette data
VCE_DATA_HI     := $0405        ; MSB of 16-bit palette data

; Programmable Sound Generator (PSG)

PSG             := $0800        ; base

PSG_CHAN_SELECT := $0800
PSG_GLOBAL_PAN  := $0801
PSG_FREQ_LO     := $0802
PSG_FREQ_HI     := $0803
PSG_CHAN_CTRL   := $0804
PSG_CHAN_PAN    := $0805
PSG_CHAN_DATA   := $0806
PSG_NOISE       := $0807
PSG_LFO_FREQ    := $0808
PSG_LFO_CTRL    := $0809

; Timer

TIMER           := $0C00        ; base

TIMER_COUNT     := $0C00
TIMER_CTRL      := $0C01

JOY_CTRL        := $1000

IRQ_MASK        := $1402
IRQ_STATUS      := $1403

CDR_MEM_DISABLE := $1803
CDR_MEM_ENABLE  := $1807

; Write to a VDC register.
.macro  VREG    arg1, arg2
        st0     #arg1
        st1     #<(arg2)
        st2     #>(arg2)
.endmacro
