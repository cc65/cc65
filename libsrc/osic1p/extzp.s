;
; Additional zero page locations for the Challenger 1P.
; NOTE: The zeropage locations contained in this file get initialized
; in the startup code, so if you change anything here, be sure to check
; not only the linker config, but also the startup file.
;

; ------------------------------------------------------------------------

        .include        "extzp.inc"

.segment        "EXTZP" : zeropage

CURS_X:         .res    1
CURS_Y:         .res    1
SCREEN_PTR:     .res    2

; size 4
; Adjust size of this segment in osic1p.cfg if the size changes
