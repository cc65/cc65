;
; Ullrich von Bassewitz, 2003-02-16
;
; Additional zero page locations for the CBM510.
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
sysp1:          .res    2
sysp3:          .res    2
vic:            .res    2
sid:            .res    2
cia1:           .res    2
cia2:           .res    2
acia:           .res    2
tpi1:           .res    2
tpi2:           .res    2
ktab1:          .res    2
ktab2:          .res    2
ktab3:          .res    2
ktab4:          .res    2

sysp0:          .word   $0000
time:           .dword  $0000
segsave:        .byte   0
ktmp:           .byte   0
CURS_X:         .byte   0
CURS_Y:         .byte   0
CURS_FLAG:      .byte   0
CURS_STATE:     .byte   0
CURS_BLINK:     .byte   0
CURS_COLOR:     .byte   0
CHARCOLOR:      .byte   0
RVS:            .byte   0
DEVNUM:         .byte   0
SCREEN_PTR:     .word   0
CRAM_PTR:       .word   0
; Stuff for our own kbd polling routine
keyidx:         .byte   0               ; Number of keys in keyboard buffer
keybuf:         .res    10              ; Keyboard buffer
keyscanbuf:     .byte   0
keysave:        .byte   0
modkey:         .byte   0
norkey:         .byte   0
graphmode:      .byte   0
lastidx:        .byte   0
rptdelay:       .byte   0
rptcount:       .byte   0

