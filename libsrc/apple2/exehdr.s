;
; Oliver Schmidt, 2012-06-10
;
; This module supplies a 4 byte DOS 3.3 header
; containing the load address and load length.
;

        .export         __EXEHDR__ : absolute = 1       ; Linker referenced
        .import         __LOADADDR__, __LOADSIZE__      ; Linker generated

; ------------------------------------------------------------------------

        .segment        "EXEHDR"

        .addr           __LOADADDR__    ; Load address
        .word           __LOADSIZE__    ; Load length
