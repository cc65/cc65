
;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char ChangeDiskDevice (char newDriveNumber);

	    .import setoserror
	    .export _ChangeDiskDevice

	    .include "jumptab.inc"
	
_ChangeDiskDevice:
	jsr ChangeDiskDevice
	jmp setoserror
