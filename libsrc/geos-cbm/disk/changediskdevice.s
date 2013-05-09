;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char ChangeDiskDevice (char newDriveNumber);

            .export _ChangeDiskDevice
            .import setoserror

            .include "jumptab.inc"
        
_ChangeDiskDevice:
        jsr ChangeDiskDevice
        jmp setoserror
