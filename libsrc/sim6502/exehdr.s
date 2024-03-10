;
; Oliver Schmidt, 2013-05-16
;
; This module supplies a header used by sim65.
;

        .export         __EXEHDR__ : absolute = 1       ; Linker referenced
        .importzp       sp
        .import         __MAIN_START__
        .import         startup

        .macpack        cpu

        .segment        "EXEHDR"

        .byte   $73, $69, $6D, $36, $35        ; 'sim65'
        .byte   2                              ; header version
.if (.cpu .bitand ::CPU_ISET_6502X)
        .byte   2
.elseif (.cpu .bitand ::CPU_ISET_65C02)
        .byte   1
.elseif (.cpu .bitand ::CPU_ISET_6502)
        .byte   0
.else
        .error Unknow CPU type.
.endif
        .byte   sp                             ; sp address
        .addr   __MAIN_START__                 ; load address
        .addr   startup                        ; reset address
