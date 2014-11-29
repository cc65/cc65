;
; Additional zero page locations for the Challenger 1P.
; NOTE: The zeropage locations contained in this file get initialized
; in the startup code, so if you change anything here, be sure to check
; not only the linker config, but also the startup file.
;

; ------------------------------------------------------------------------

        .include        "extzp.inc"

.segment        "EXTZP" : zeropage

CURS_X:         .byte   0
CURS_Y:         .byte   0
CURS_SAV:       .byte   0
SCR_LINELEN:    .byte   24
SCREEN_PTR:     .res    2

; size 6
; Adjust size of this segment in c1p.cfg if the size changes
