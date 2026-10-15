;
; 2026 cc65 project (added for PRINTER driver support, see grc65)
;

; char __fastcall__ StopPrint (char *tempBuf, char *workBuf);

            .export _StopPrint
            .import popax

            .include "printdrv.inc"
            .include "geossym.inc"

_StopPrint:
        sta r1L                 ; workBuf -- passed in A/X (fastcall, pointer)
        stx r1H
        jsr popax               ; tempBuf
        sta r0L
        stx r0H
        jsr StopPrint
        txa
        rts
