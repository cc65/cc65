;
; Maciej 'YTM/Elysium' Witkowiak
;
; 21.12.1999, 2.1.2003

; char NxtBlkAlloc (struct tr_se *startTS, struct tr_se output[], int length );

            .import popax, setoserror
            .import gettrse
            .importzp ptr4
            .export _NxtBlkAlloc

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_NxtBlkAlloc:
        sta r2L
        stx r2H
        jsr popax
        sta r4L
        stx r4H
        jsr popax
        jsr gettrse
        sta r3L
        stx r3H
        jsr NxtBlkAlloc
        jmp setoserror
