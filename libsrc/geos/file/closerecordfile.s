
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 25.12.99

; char CloseRecordFile  (void);

	    .export _CloseRecordFile

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_CloseRecordFile:
	jsr CloseRecordFile
	stx errno
	txa
	rts
