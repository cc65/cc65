;
; Ullrich von Bassewitz, 31.05.1998
;
; Data for the stdio file stream.
;
; Be sure to keep the value priority of closeallstreams lower than that of
; closeallfiles (which is the low level POSIX counterpart and must be
; called after closeallstreams).

	.export	 	__filetab, _stdin, _stdout, _stderr
        .destructor     closeallstreams, 16
        .import         _close

        .include        "fcntl.inc"
        .include        "_file.inc"

;----------------------------------------------------------------------------
; Close all files on exit

.proc   closeallstreams

       	ldy	#((FOPEN_MAX - 1) * _FILE_size)
loop:  	sty     index                           ; Save the index
        lda     __filetab + _FILE_f_flags,y     ; Load file flags
        and     #_FOPEN                         ; Is it open?
       	beq    	next                            ; jump if closed

; Close this file

        lda     __filetab + _FILE_f_fd,y
        ldx     #0
        jsr     _close

; Next file

next:   lda     index
        sec
        sbc     #_FILE_size
        tay
        bcs     loop

        rts

.endproc

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


; Temp storage for closeallstreams

.bss
index:  .res    1

