
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char OpenRecordFile  (char *myName);

	    .import setoserror
	    .export _OpenRecordFile

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_OpenRecordFile:
	sta r0L
	stx r0H
	jsr OpenRecordFile
	jmp setoserror
