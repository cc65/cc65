
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char PointRecord  (char recordNum);

	    .import __oserror
	    .export _PointRecord

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_PointRecord:
	jsr PointRecord
	stx __oserror
	txa
	rts
