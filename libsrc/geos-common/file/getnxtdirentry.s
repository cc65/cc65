;
; Maciej 'YTM/Alliance' Witkowiak
;
; 26.10.99

; struct filehandle* GetNxtDirEntry (void);

            .import __oserror, return0
            .export _GetNxtDirEntry

            .include "diskdrv.inc"
            .include "geossym.inc"

_GetNxtDirEntry:
        jsr GetNxtDirEntry
        stx __oserror
        txa
        beq L0                  ; error?
        tya
        beq L0                  ; end of dir?
        jmp return0             ; return NULL

L0:     lda r5L
        ldx r5H
        rts
