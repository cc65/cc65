;
; Maciej 'YTM/Alliance' Witkowiak
;
; 21.12.99

; struct tr_se SetNextFree (struct tr_se *startTS);

            .import __oserror
            .import gettrse
            .export _SetNextFree

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_SetNextFree:
        jsr gettrse
        sta r3L
        stx r3H
        jsr SetNextFree
        stx __oserror
        lda r3L
        ldx r3H
        rts
