
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char FreeFile  (struct trse myTrSe[]);

	    .import __oserror
	    .export _FreeFile

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_FreeFile:
	sta r9L
	stx r9H
	jsr FreeFile
	stx __oserror
	txa
	rts
