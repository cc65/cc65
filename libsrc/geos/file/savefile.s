
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char SaveFile  (struct fileheader *myHeader);

	    .import setoserror
	    .export _SaveFile

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_SaveFile:
	sta r9L
	stx r9H
	jsr SaveFile
	jmp setoserror
