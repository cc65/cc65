
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char ChangeDiskDevice (char newDriveNumber);

	    .export _ChangeDiskDevice

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_ChangeDiskDevice:
	jsr ChangeDiskDevice
	stx errno
	txa
	rts
