
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char PointRecord  (char recordNum);

	    .import setoserror
	    .export _PointRecord

	    .include "jumptab.inc"
	
_PointRecord:
	jsr PointRecord
	jmp setoserror
