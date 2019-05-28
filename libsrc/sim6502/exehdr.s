;
; Oliver Schmidt, 2013-05-16
;
; This module supplies a 2 byte header identifying the simulator type,
; and parameter stack pointer sp.
;

        .export         __EXEHDR__ : absolute = 1       ; Linker referenced
        .importzp       sp

        .segment        "EXEHDR"

        .byte   .defined(__SIM65C02__)
        .byte   sp
