;
; Ullrich von Bassewitz, 31.05.1998
;
; Data for the stdio file stream.
;

	.export		__filetab, _stdin, _stdout, _stderr

.data

__filetab:
in: 	.byte	0, 1   		; stdin
out:	.byte	1, 1   		; stdout
err:	.byte	2, 1   		; stderr
      	.byte	0, 0   		; free slot
      	.byte	0, 0   		; free slot
      	.byte	0, 0   		; free slot
      	.byte	0, 0   		; free slot
      	.byte	0, 0   		; free slot

_stdin:
	.word	in

_stdout:
	.word	out

_stderr:
	.word	err
