
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char DeleteRecord  (void);

	    .import setoserror
	    .export _DeleteRecord

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_DeleteRecord:
	jsr DeleteRecord
	jmp setoserror
