;
; Additional zero page locations for the Challenger 1P.
; NOTE: The zeropage locations contained in this file get initialized
; in the startup code, so if you change anything here, be sure to check
; not only the linker config, but also the startup file.
;

; ------------------------------------------------------------------------

        .include        "extzp.inc"

.segment        "EXTZP" : zeropage

; The following values get initialized from a table in the startup code.
; While this sounds crazy, it has reasons that have to do with modules (and
; we have the space anyway). So when changing anything, be sure to adjust the
; initializer table
CURS_X:         .byte   0
CURS_Y:         .byte   0
SCR_LINELEN:    .byte   24
SCREEN_PTR:     .res    2

; size 5
