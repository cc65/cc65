
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char DeleteFile  (char *myName);

	    .import setoserror
	    .export _DeleteFile

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_DeleteFile:
	sta r0L
	stx r0H
	jsr DeleteFile
	jmp setoserror
