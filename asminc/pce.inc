;
; PCE definitions. By Groepaz/Hitmem.
;

; FIXME: screen dimensions my change according to selected video mode
screenrows      = (224/8)
charsperline    = 61

CH_HLINE        = 1
CH_VLINE        = 2

; huc6270 - Video Display Controller (VDC)

VDC_MAWR        = 0             ; Memory Address Write Register
VDC_MARR        = 1             ; Memory Address Read Register
VDC_VWR         = 2             ; VRAM Write Register (write only)
VDC_VRR         = 2             ; VRAM Read Register (read only)
VDC_UNK03       = 3             ; (unknown)
VDC_UNK04       = 4             ; (unknown)
VDC_CR          = 5             ; Control Register
VDC_RCR         = 6             ; Raster Counter Register
VDC_BXR         = 7             ; Background X-Scroll Register
VDC_BYR         = 8             ; Background Y-Scroll Register
VDC_MWR         = 9             ; Memory-access Width Register
VDC_HSR         = 10            ; Horizontal Sync Register
VDC_HDR         = 11            ; Horizontal Display Register
VDC_VPR         = 12            ; Vertical synchronous register
VDC_VDW         = 13            ; Vertical display register
VDC_VCR         = 14            ; Vertical display END position register
VDC_DCR         = 15            ; (DMA) Control Register
VDC_SOUR        = 16            ; (DMA) Source Register
VDC_DESR        = 17            ; (DMA) Destination Register
VDC_LENR        = 18            ; (DMA) Length Register
VDC_SATB        = 19            ; Sprite Attribute Table

; VDC port
; Note: absolute addressing mode must be used when writing to this port

VDC_CTRL        = $0000
VDC_DATA_LO     = $0002
VDC_DATA_HI     = $0003

; huc6260 - Video Color Encoder (vce)

; The DAC has a palette of 512 colours.
; bitmap of the palette data is this: 0000000gggrrrbbb.
; You can read and write the DAC-registers.

VCE             = $0400         ; base

VCE_CTRL        = $0400         ; write$00 to reset
VCE_ADDR_LO     = $0402         ; LSB of byte offset into palette
VCE_ADDR_HI     = $0403         ; MSB of byte offset into palette
VCE_DATA_LO     = $0404         ; LSB of 16-bit palette data
VCE_DATA_HI     = $0405         ; MSB of 16-bit palette data

; programmable sound generator (PSG)

PSG             = $0800         ; base

PSG_CHAN_SELECT = $0800
PSG_GLOBAL_PAN  = $0801
PSG_FREQ_LO     = $0802
PSG_FREQ_HI     = $0803
PSG_CHAN_CTRL   = $0804
PSG_CHAN_PAN    = $0805
PSG_CHAN_DATA   = $0806
PSG_NOISE       = $0807
PSG_LFO_FREQ    = $0808
PSG_LFO_CTRL    = $0809

; timer

TIMER           = $0c00         ; base

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
