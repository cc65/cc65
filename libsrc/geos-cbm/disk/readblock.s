
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char ReadBlock (struct tr_se myTS, char *buffer);

	    .import popax, setoserror
	    .import gettrse
	    .export _ReadBlock

	    .include "jumptab.inc"
	    .include "geossym.inc"
	
_ReadBlock:
	sta r4L
	stx r4H
	jsr popax
	jsr gettrse
	sta r1L
	stx r1H
	jsr ReadBlock
	jmp setoserror
