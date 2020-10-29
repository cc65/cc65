;
; Maciej 'YTM/Alliance' Witkowiak
;
; 26.10.99

; struct filehandle* Get1stDirEntry (void);

            .import __oserror, return0
            .export _Get1stDirEntry

            .include "diskdrv.inc"
            .include "geossym.inc"

_Get1stDirEntry:
        jsr Get1stDirEntry
        stx __oserror
        txa
        beq L0                  ; error?
        jmp return0             ; return NULL

L0:     lda r5L
        ldx r5H
        rts
