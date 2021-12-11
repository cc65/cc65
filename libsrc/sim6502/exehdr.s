;
; Oliver Schmidt, 2013-05-16
;
; This module supplies a header used by sim65.
;

        .export         __EXEHDR__ : absolute = 1       ; Linker referenced
        .importzp       sp
        .import         __MAIN_START__
        .import         startup

        .segment        "EXEHDR"

        .byte   $73, $69, $6D, $36, $35        ; 'sim65'
        .byte   2                              ; header version
        .byte   .defined(__SIM65C02__)         ; CPU type
        .byte   sp                             ; sp address
        .addr   __MAIN_START__                 ; load address
        .addr   startup                        ; reset address
