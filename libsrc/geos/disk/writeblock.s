
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char WriteBlock (struct tr_se *myTS, char *buffer);

	    .import popax
	    .import gettrse
	    .export _WriteBlock

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_WriteBlock:
	sta r4L
	stx r4H
	jsr popax
	jsr gettrse
	sta r1L
	stx r1H
	jsr WriteBlock
	stx errno
	txa
	rts
