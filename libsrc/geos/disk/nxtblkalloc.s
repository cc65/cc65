
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char NxtBlkAlloc (struct tr_se *startTS, struct tr_se output[], int length );

	    .import popax, __oserror
	    .import gettrse
	    .importzp ptr4
	    .export _NxtBlkAlloc

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_NxtBlkAlloc:
	sta r2L
	stx r2H
	jsr popax
	sta r4L
	stx r4H
	jsr popax
	jsr gettrse
	sta r3L
	stx r3H
	jsr NxtBlkAlloc
	stx __oserror
	txa
	rts
