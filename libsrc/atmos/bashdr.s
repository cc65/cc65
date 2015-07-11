;
; 2010-11-14, Ullrich von Bassewitz
; 2014-09-06, Greg King
;
; This module supplies a small BASIC stub program that uses CALL
; to jump to the machine-language code that follows it.
;

        ; The following symbol is used by the linker config. file
        ; to force this module to be included into the output file.
        .export __BASHDR__:abs = 1


.segment        "BASHDR"

        .addr   Next
        .word   .version        ; Line number
        .byte   $BF,'#'         ; CALL token, mark number as hexadecimal
        .byte   <(Start >> 8      ) + '0' + (Start >> 8       > $09) * $07
        .byte   <(Start >> 4 & $0F) + '0' + (Start >> 4 & $0F > $09) * $07
        .byte   <(Start      & $0F) + '0' + (Start      & $0F > $09) * $07
        .byte   $00             ; End of BASIC line
Next:   .addr   $0000           ; BASIC program end marker
Start:
