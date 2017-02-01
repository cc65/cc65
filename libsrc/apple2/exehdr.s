;
; Oliver Schmidt, 2012-06-10
;
; This module supplies a 4 byte DOS 3.3 header
; containing the load address and load length.
;

        .export         __EXEHDR__ : absolute = 1       ; Linker referenced
        .import         __MAIN_START__, __MAIN_LAST__   ; Linker generated

; ------------------------------------------------------------------------

        .segment        "EXEHDR"

        .addr           __MAIN_START__                  ; Load address
        .word           __MAIN_LAST__ - __MAIN_START__  ; Load length
