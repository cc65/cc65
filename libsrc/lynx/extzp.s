;
; Ullrich von Bassewitz, 2004-11-06
;
; zeropage locations for exclusive use by the library
;

        .include "extzp.inc"

        .segment "EXTZP" : zeropage

; ------------------------------------------------------------------------
; mikey and suzy shadow registers

__iodat:    .res    1
__iodir:    .res    1
__viddma:   .res    1
__sprsys:   .res    1


