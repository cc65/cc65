
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char DeleteFile  (char *myName);

	    .export _DeleteFile

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_DeleteFile:
	sta r0L
	stx r0H
	jsr DeleteFile
	stx errno
	txa
	rts
