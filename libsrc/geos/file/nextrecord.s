
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char NextRecord  (void);

	    .import __oserror
	    .export _NextRecord

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_NextRecord:
	jsr NextRecord
	stx __oserror
	txa
	rts
