
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char FindFile  (char *myName);

	    .import __oserror
	    .export _FindFile

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_FindFile:
	sta r6L
	stx r6H
	jsr FindFile
	stx __oserror
	txa
	rts
