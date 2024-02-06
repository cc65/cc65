;
; Oliver Schmidt, 30.12.2004
;
; File descriptor management for the POSIX I/O routines
;

        .struct FD
                REF_NUM .byte
                FLAGS   .byte
                BUFFER  .addr
        .endstruct

        .global         fdtab
        .global         getfd

MAX_FDS = 8
