
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char ChkDkGEOS (void);

	    .import __oserror
	    .export _ChkDkGEOS

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_ChkDkGEOS:
	jsr ChkDkGEOS
	stx __oserror
	lda isGEOS
	rts
