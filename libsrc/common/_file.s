;
; Ullrich von Bassewitz, 31.05.1998
;
; Data for the stdio file stream.
;

       	.export	       	__filetab
                                 
        .include        "stdio.inc"
        .include        "fcntl.inc"
        .include        "_file.inc"

;----------------------------------------------------------------------------
; File data

.data

__filetab:
        .byte   0, _FOPEN       ; stdin
        .byte   1, _FOPEN       ; stdout
        .byte   2, _FOPEN       ; stderr
.repeat FOPEN_MAX - 3
        .byte   0, _FCLOSED     ; free slot
.endrepeat


; Standard file descriptors

_stdin:
	.word	__filetab + (STDIN_FILENO * _FILE_size)

_stdout:
	.word	__filetab + (STDOUT_FILENO * _FILE_size)

_stderr:
	.word	__filetab + (STDERR_FILENO * _FILE_size)


