
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 26.10.99

; char WriteBuff (struct tr_se*);

	    .import gettrse
	    .export _WriteBuff

	    .include "../inc/diskdrv.inc"
	    .include "../inc/geossym.inc"
	
_WriteBuff:
	jsr gettrse
	sta r1L
	stx r1H
	jsr WriteBuff
	stx errno
	txa
	rts
