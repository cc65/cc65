
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 26.10.99

; struct filehandle* Get1stDirEntry (void);

	    .export _Get1stDirEntry

	    .include "../inc/diskdrv.inc"
	    .include "../inc/geossym.inc"

_Get1stDirEntry:
	jsr Get1stDirEntry
	stx errno
	lda r5L
	ldx r5H
	rts
