
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char OpenRecordFile  (char *myName);

	    .import __oserror
	    .export _OpenRecordFile

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_OpenRecordFile:
	sta r0L
	stx r0H
	jsr OpenRecordFile
	stx __oserror
	txa
	rts
