;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char WriteRecord  (char *buffer, int length);

            .export _WriteRecord
            .import popax, setoserror

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_WriteRecord:
        sta r2L
        stx r2H
        jsr popax
        sta r7L
        stx r7H
        jsr WriteRecord
        jmp setoserror
