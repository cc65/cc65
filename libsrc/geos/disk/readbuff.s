
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 26.10.99

; char ReadBuff  (struct tr_se);

	    .import __oserror
	    .import gettrse
	    .export _ReadBuff

	    .include "../inc/diskdrv.inc"
	    .include "../inc/geossym.inc"
	
_ReadBuff:
	jsr gettrse
	sta r1L
	stx r1H
	jsr ReadBuff
	stx __oserror
	txa
	rts
