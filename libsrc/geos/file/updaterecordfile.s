
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char UpdateRecordFile  (void);

	    .export _UpdateRecordFile

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_UpdateRecordFile:
	jsr UpdateRecordFile
	stx errno
	txa
	rts
