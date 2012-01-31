
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 26.10.1999, 2.1.2003

; char ReadBuff  (struct tr_se);

	    .import setoserror
	    .import gettrse
	    .export _ReadBuff

	    .include "diskdrv.inc"
	    .include "geossym.inc"
	
_ReadBuff:
	jsr gettrse
	sta r1L
	stx r1H
	jsr ReadBuff
	jmp setoserror
