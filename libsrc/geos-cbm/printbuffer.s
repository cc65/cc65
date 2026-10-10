;
; 2026 cc65 project (added for PRINTER driver support, see grc65)
;

; void __fastcall__ PrintBuffer (char *printData, char *workBuf, char *colorData);

            .export _PrintBuffer
            .import popax

            .include "printdrv.inc"
            .include "geossym.inc"

_PrintBuffer:
        sta r2L                 ; colorData -- passed in A/X (fastcall, pointer)
        stx r2H
        jsr popax               ; workBuf
        sta r1L
        stx r1H
        jsr popax               ; printData
        sta r0L
        stx r0H
        jsr PrintBuffer
        rts
