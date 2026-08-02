;
; Oliver Schmidt, 30.12.2004
; Robert Justice, 2026
;
; File descriptor management for the POSIX I/O routines
;

        .struct FD
                REF_NUM .byte
                FLAGS   .byte
        .endstruct

        .global         fdtab
        .global         getfd

MAX_FDS = 16
