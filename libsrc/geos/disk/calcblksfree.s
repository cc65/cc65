
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; int CalcBlksFree (void);

	    .export _CalcBlksFree

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_CalcBlksFree:
	jsr CalcBlksFree
	stx errno
	lda r4L
	ldx r4H
	rts
