
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char ChkDkGEOS (void);

	    .import setoserror
	    .export _ChkDkGEOS

	    .include "jumptab.inc"
	    .include "geossym.inc"
	
_ChkDkGEOS:
	jsr ChkDkGEOS
	jsr setoserror
	lda isGEOS
	rts
