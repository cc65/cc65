
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char GetFHdrInfo  (struct filehandle *myFile);

	    .export _GetFHdrInfo

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_GetFHdrInfo:
	sta r9L
	stx r9H
	jsr GetFHdrInfo
	stx errno
	txa
	rts
