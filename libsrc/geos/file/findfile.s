
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char FindFile  (char *myName);

	    .export _FindFile

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_FindFile:
	sta r6L
	stx r6H
	jsr FindFile
	stx errno
	txa
	rts
