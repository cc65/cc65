
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 30.10.99

; int GetSerialNumber (void);

	    .export _GetSerialNumber

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"

_GetSerialNumber:

	    jsr GetSerialNumber
	    lda r0L
	    ldx r0H
	    rts
