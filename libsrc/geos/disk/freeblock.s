
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char FreeBlock (struct tr_se *TS);

	    .import gettrse
	    .export _FreeBlock

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_FreeBlock:
	jsr gettrse
	sta r6L
	stx r6H
	jsr FreeBlock
	stx errno
	txa
	rts
