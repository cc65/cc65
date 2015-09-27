
; ram under i/o
soft80_lo_charset       = $d000
soft80_hi_charset       = $d400
soft80_vram             = $d800
soft80_colram           = $d800 ; color ram (used for temp. storage)
; ram under kernel
soft80_bitmap           = $e000

charsperline            = 80
screenrows              = 25

CH_ESC                  = 95
CH_HLINE                = 96
CH_CROSS                = 123
CH_VLINE                = 125
CH_PI                   = 126

; FIXME: these are defined in cbm.h normally, the putchar stuff should accept
;        the regular codes instead of the following ones:

CH_LTEE                 = 171-160
CH_URCORNER             = 174-160
CH_LLCORNER             = 173-160
CH_ULCORNER             = 176-160
CH_BTEE                 = 177-160
CH_TTEE                 = 178-160
CH_RTEE                 = 179-160
CH_LRCORNER             = 189-160
