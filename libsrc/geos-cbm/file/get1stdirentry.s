
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 26.10.99

; struct filehandle* Get1stDirEntry (void);

	    .import __oserror
	    .export _Get1stDirEntry

	    .include "../inc/diskdrv.inc"
	    .include "../inc/geossym.inc"

_Get1stDirEntry:
	jsr Get1stDirEntry
	stx __oserror
	lda r5L
	ldx r5H
	rts
