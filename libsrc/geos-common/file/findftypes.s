;
; Maciej 'YTM/Elysium' Witkowiak
;
; 25.12.1999, 2.1.2003

; char FindFTypes  (char *buffer, char fileType, char fileMax, char *Class);

            .export _FindFTypes
            .import popax, popa, setoserror

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"

_FindFTypes:
        sta r10L
        stx r10H
        jsr popa
        sta r7H
        sta tmpFileMax
        jsr popa
        sta r7L
        jsr popax
        sta r6L
        stx r6H
        jsr FindFTypes
        jsr setoserror
; return (fileMax - r7H)
        lda tmpFileMax
        sec
        sbc r7H
        rts

.bss

tmpFileMax:
        .res 1
