
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char PutBlock (struct tr_se *myTS, char *buffer);

	    .import popax
	    .import gettrse
	    .export _PutBlock

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_PutBlock:
	sta r4L
	stx r4H
	jsr popax
	jsr gettrse
	sta r1L
	stx r1H
	jsr PutBlock
	stx errno
	txa
	rts
