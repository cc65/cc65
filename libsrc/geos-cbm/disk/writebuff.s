
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 26.10.1999, 2.1.2003

; char WriteBuff (struct tr_se*);

	    .import setoserror
	    .import gettrse
	    .export _WriteBuff

	    .include "../inc/diskdrv.inc"
	    .include "../inc/geossym.inc"
	
_WriteBuff:
	jsr gettrse
	sta r1L
	stx r1H
	jsr WriteBuff
	jmp setoserror
