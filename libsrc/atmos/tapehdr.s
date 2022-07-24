;
; Based on code by Debrune J�r�me <jede@oric.org>
; 2016-03-17, Greg King
;

        ; The following symbol is used by the linker config. file
        ; to force this module to be included into the output file.
        .export __TAPEHDR__:abs = 1

        ; These symbols, also, come from the configuration file.
        .import __AUTORUN__, __PROGFLAG__
        .import __BASHEAD_START__, __MAIN_LAST__


; ------------------------------------------------------------------------
; Oric cassette-tape header

.segment        "TAPEHDR"

        .byte   $16, $16, $16           ; Sync bytes
        .byte   $24                     ; Beginning-of-header marker

        .byte   $00                     ; $2B0
        .byte   $00                     ; $2AF
        .byte   <__PROGFLAG__           ; $2AE Language flag ($00=BASIC, $80=machine code)
        .byte   <__AUTORUN__            ; $2AD Auto-run flag ($C7=run, $00=only load)
        .dbyt   __MAIN_LAST__ - 1       ; $2AB Address of end of file
        .dbyt   __BASHEAD_START__       ; $2A9 Address of start of file
        .byte   $00                     ; $2A8

        ; File name (a maximum of 17 characters), zero-terminated
        .asciiz .sprintf("%u", .time)
