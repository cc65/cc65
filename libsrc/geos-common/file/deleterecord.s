;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char DeleteRecord  (void);

            .import setoserror
            .export _DeleteRecord

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_DeleteRecord:
        jsr DeleteRecord
        jmp setoserror
