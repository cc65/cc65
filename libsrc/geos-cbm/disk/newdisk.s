;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char NewDisk (void);

            .export _NewDisk
            .import setoserror

            .include "jumptab.inc"
        
_NewDisk:
        jsr NewDisk
        jmp setoserror
