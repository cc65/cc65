
;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; char ChangeDiskDevice (char newDriveNumber);

	    .import __oserror
	    .export _ChangeDiskDevice

	    .include "../inc/jumptab.inc"
	    .include "../inc/geossym.inc"
	
_ChangeDiskDevice:
	jsr ChangeDiskDevice
	stx __oserror
	txa
	rts
