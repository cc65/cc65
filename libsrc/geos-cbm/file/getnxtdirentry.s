
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 26.10.99

; struct filehandle* GetNxtDirEntry (void);

	    .import __oserror
	    .export _GetNxtDirEntry

	    .include "../inc/diskdrv.inc"
	    .include "../inc/geossym.inc"

_GetNxtDirEntry:
	jsr GetNxtDirEntry
	stx __oserror
	lda r5L
	ldx r5H
	rts
