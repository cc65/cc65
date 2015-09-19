;
; Groepaz/Hitmen, 2015-11-19
;
; zeropage locations for exclusive use by the library
;

        .include "extzp.inc"

        .segment "EXTZP" : zeropage

CURS_X:         .res 1
CURS_Y:         .res 1
SCREEN_PTR:     .res 2
CHARCOLOR:      .res 1
RVS:            .res 1
BGCOLOR:        .res 1
tickcount:      .res 4
vdc_flags:      .res 1
