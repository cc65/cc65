
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char AppendRecord  (void);

	    .import __oserror
	    .export _AppendRecord

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_AppendRecord:

	jsr AppendRecord
	stx __oserror
	txa
	rts
