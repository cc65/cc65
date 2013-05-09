;
; Oliver Schmidt, 30.12.2004
;
; File descriptor management for the POSIX I/O routines
;

        .include        "errno.inc"
        .include        "fcntl.inc"
        .include        "filedes.inc"

getfd:
        ; Check for handle >= 256
        cpx     #$00
        bne     error

        ; Check for handle >= MAX_FDS
        cmp     #MAX_FDS
        bcs     error

        ; Convert handle to fdtab slot
        .assert .sizeof(FD) = 4, error
        asl
        asl

        ; Check for fdtab slot in use
        tay
        lda     fdtab + FD::REF_NUM,y
        beq     error

        ; Return success
        clc
        rts

        ; Load errno code and return error
error:  lda     #EINVAL
        sec
        rts

        .data

fdtab:  .assert .sizeof(FD) = 4, error

        .byte   $80             ; STDIN_FILENO ::REF_NUM
        .byte   O_RDONLY        ; STDIN_FILENO ::FLAGS
        .addr   $0000           ; STDIN_FILENO ::BUFFER

        .byte   $80             ; STDOUT_FILENO::REF_NUM
        .byte   O_WRONLY        ; STDOUT_FILENO::FLAGS
        .addr   $0000           ; STDOUT_FILENO::BUFFER

        .byte   $80             ; STDERR_FILENO::REF_NUM
        .byte   O_WRONLY        ; STDERR_FILENO::FLAGS
        .addr   $0000           ; STDERR_FILENO::BUFFER

        .res    (MAX_FDS - 3) * .sizeof(FD)
