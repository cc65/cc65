
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char BlkAlloc (struct tr_se output[], int length);

	    .import popax, __oserror
	    .export _BlkAlloc

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_BlkAlloc:
	sta r2L
	stx r2H
	jsr popax
	sta r4L
	stx r4H
	jsr BlkAlloc
	stx __oserror
	txa
	rts
