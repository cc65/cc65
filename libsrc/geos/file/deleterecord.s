
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char DeleteRecord  (void);

	    .import __oserror
	    .export _DeleteRecord

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_DeleteRecord:
	jsr DeleteRecord
	stx __oserror
	txa
	rts
