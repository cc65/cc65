
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char UpdateRecordFile  (void);

	    .import setoserror
	    .export _UpdateRecordFile

	    .include "jumptab.inc"

_UpdateRecordFile:
	jsr UpdateRecordFile
	jmp setoserror
