;
; Oliver Schmidt, 2013-05-16
;
; This module supplies a 1 byte header identifying the simulator type
;

        .export         __EXEHDR__ : absolute = 1       ; Linker referenced

        .segment        "EXEHDR"

        .byte   .defined(__SIM65C02__)
