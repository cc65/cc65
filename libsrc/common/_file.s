;
; Ullrich von Bassewitz, 31.05.1998
;
; Data for the stdio file stream.
;

        .export         __filetab

        .include        "stdio.inc"
        .include        "fcntl.inc"
        .include        "_file.inc"

;----------------------------------------------------------------------------
; File data

.data

__filetab:
        .byte   0, _FOPEN, 0    ; stdin
        .byte   1, _FOPEN, 0    ; stdout
        .byte   2, _FOPEN, 0    ; stderr
.repeat FOPEN_MAX - 3
        .byte   0, _FCLOSED, 0  ; free slot
.endrepeat


; Standard file descriptors

_stdin:
        .word   __filetab + (STDIN_FILENO * .sizeof(_FILE))

_stdout:
        .word   __filetab + (STDOUT_FILENO * .sizeof(_FILE))

_stderr:
        .word   __filetab + (STDERR_FILENO * .sizeof(_FILE))


