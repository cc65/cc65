;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char CloseRecordFile  (void);

            .import setoserror
            .export _CloseRecordFile

            .include "jumptab.inc"
            .include "diskdrv.inc"
        
_CloseRecordFile:
        jsr CloseRecordFile
        jmp setoserror
