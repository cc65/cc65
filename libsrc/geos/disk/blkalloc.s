
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char BlkAlloc (struct tr_se output[], int length);

	    .import popax, setoserror
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
	jmp setoserror
