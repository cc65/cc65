
soft80_lo_charset       = $d000
soft80_hi_charset       = $d400
soft80_vram             = $d800 ; ram under i/o
soft80_colram           = $d800 ; color ram (used for temp. storage)
soft80_bitmap           = $e000

charsperline            = 80
screenrows              = 25

CH_ESC                  = 95
CH_HLINE                = 96
CH_CROSS                = 123
CH_VLINE                = 125
CH_PI                   = 126
CH_LTEE                 = 171
CH_URCORNER             = 174
CH_LLCORNER             = 173
CH_ULCORNER             = 176
CH_BTEE                 = 177
CH_TTEE                 = 178
CH_RTEE                 = 179
CH_LRCORNER             = 189

;-------------------------------------------------------------------------------
; set to 0 to disable the color-ram "voodoo" for debugging purposes
.define SOFT80COLORVOODOO 1
; set to 0 to disable special case optimization for the "space" character
.define SOFT80FASTSPACE 1

