;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char AppendRecord  (void);

            .import setoserror
            .export _AppendRecord

            .include "jumptab.inc"
            .include "diskdrv.inc"
        
_AppendRecord:

        jsr AppendRecord
        jmp setoserror
