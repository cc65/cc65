
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char ChkDkGEOS (void);

	    .export _ChkDkGEOS

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_ChkDkGEOS:
	jsr ChkDkGEOS
	stx errno
	lda isGEOS
	rts
