
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char ReadBlock (struct tr_se myTS, char *buffer);

	    .import popax, __oserror
	    .import gettrse
	    .export _ReadBlock

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_ReadBlock:
	sta r4L
	stx r4H
	jsr popax
	jsr gettrse
	sta r1L
	stx r1H
	jsr ReadBlock
	stx __oserror
	txa
	rts
