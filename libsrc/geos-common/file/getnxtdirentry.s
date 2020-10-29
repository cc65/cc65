;
; 1999-10-26, Maciej 'YTM/Alliance' Witkowiak
; 2020-10-29, Greg King
;

; struct filehandle* GetNxtDirEntry (void);

            .import __oserror, return0
            .export _GetNxtDirEntry

            .include "diskdrv.inc"
            .include "geossym.inc"

_GetNxtDirEntry:
        jsr GetNxtDirEntry
        stx __oserror
        txa
        bne L1                  ; jump if disk error
        tya
        bne L1                  ; jump when no more entries
        lda r5L
        ldx r5H
        rts

L1:     jmp return0             ; return NULL if not valid entry
