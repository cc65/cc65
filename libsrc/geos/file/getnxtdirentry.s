
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 26.10.99

; struct filehandle* GetNxtDirEntry (void);

	    .export _GetNxtDirEntry

	    .include "../inc/diskdrv.inc"
	    .include "../inc/geossym.inc"

_GetNxtDirEntry:
	jsr GetNxtDirEntry
	stx errno
	lda r5L
	ldx r5H
	rts
