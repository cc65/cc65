;
; Ullrich von Bassewitz, 31.05.1998
;
; Data for the stdio file stream.
;

	.export		__filetab, _stdin, _stdout, _stderr

        .include        "_file.inc"

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
	.word	__filetab + (0 * _FILE_size)

_stdout:
	.word	__filetab + (1 * _FILE_size)

_stderr:
	.word	__filetab + (2 * _FILE_size)
