
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char GetBlock (struct tr_se *myTS, char *buffer);

	    .import popax, __oserror
	    .import gettrse
	    .export _GetBlock

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_GetBlock:
	sta r4L
	stx r4H
	jsr popax
	jsr gettrse
	sta r1L
	stx r1H
	jsr GetBlock
	stx __oserror
	txa
	rts
