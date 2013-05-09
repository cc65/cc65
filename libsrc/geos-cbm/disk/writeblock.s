;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char WriteBlock (struct tr_se *myTS, char *buffer);

            .export _WriteBlock
            .import popax, setoserror
            .import gettrse

            .include "jumptab.inc"
            .include "geossym.inc"
        
_WriteBlock:
        sta r4L
        stx r4H
        jsr popax
        jsr gettrse
        sta r1L
        stx r1H
        jsr WriteBlock
        jmp setoserror
