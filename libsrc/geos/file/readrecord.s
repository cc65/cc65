
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char ReadRecord  (char *buffer, int length);

	    .export _ReadRecord
	    .import popax, __oserror

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_ReadRecord:
	sta r2L
	stx r2H
	jsr popax
	sta r7L
	stx r7H
	jsr ReadRecord
	stx __oserror
	txa
	rts
