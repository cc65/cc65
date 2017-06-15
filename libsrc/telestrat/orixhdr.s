;
; By Debrune Jérôme <jede@oric.org>

;

    ; The following symbol is used by the linker config. file
    ; to force this module to be included into the output file.
    .export __ORIXHDR__:abs = 1

    ; These symbols, also, come from the configuration file.
    .import __AUTORUN__, __PROGFLAG__
    .import __BASHEAD_START__, __MAIN_LAST__


; ------------------------------------------------------------------------
; Orix header see http://orix.oric.org/doku.php?id=orix:header for specs

.segment        "ORIXHDR"

    .byte   $01, $00                ; non C64 marker (same as o65 format)

    .byte   "ori"                   ; magic number

    .byte   $01                     ; version of the header
    .byte   $00,%00000000           ; 6502 only
    .byte   $00,$00                 ; type of language
    .byte   $00,$00                 ; OS version 

    .byte   $00                     ;  reserved
    .byte   $00                     ;  auto or not

    .word   __BASHEAD_START__       ; Address of start of file
    .word   __MAIN_LAST__ - 1       ;  Address of end of file
    .word   __BASHEAD_START__       ;  Address of start of file

