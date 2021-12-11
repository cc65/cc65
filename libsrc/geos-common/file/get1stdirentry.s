;
; 1999-10-26, Maciej 'YTM/Alliance' Witkowiak
; 2020-10-29, Greg King
;

; struct filehandle* Get1stDirEntry (void);

            .import __oserror, return0
            .export _Get1stDirEntry

            .include "diskdrv.inc"
            .include "geossym.inc"

_Get1stDirEntry:
        jsr Get1stDirEntry
        stx __oserror
        txa
        bne L1                  ; jump if disk error
        lda r5L
        ldx r5H
        rts

L1:     jmp return0             ; return NULL if not valid entry
