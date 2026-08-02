;
; Robert Justice, 2024-03-18
;
; This module supplies a SOS Interpreter header
;

        .export         __EXEHDR__ : absolute = 1       ; Linker referenced
        .import         __MAIN_START__, __MAIN_LAST__   ; Linker generated

; ------------------------------------------------------------------------

;CODE_LENGTH = __MAIN_LAST__ - __MAIN_START__

; ------------------------------------------------------------------------

        .segment        "EXEHDR"

START:  .byte           "SOS NTRP"                          ; SOS Interpreter label
        .word           $0000                               ; Option header length
        .word           __MAIN_START__                      ; Code start
        .word           __MAIN_LAST__ - __MAIN_START__      ; code length
