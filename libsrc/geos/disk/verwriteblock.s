
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char VerWriteBlock (struct tr_se *myTS, char *buffer);

	    .import popax
	    .import gettrse
	    .export _VerWriteBlock

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_VerWriteBlock:
	sta r4L
	stx r4H
	jsr popax
	jsr gettrse
	sta r1L
	stx r1H
	jsr VerWriteBlock
	stx errno
	txa
	rts
