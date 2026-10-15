;
; 2026 cc65 project (added for PRINTER driver support, see grc65)
;

; void __fastcall__ PrintASCII (const char *printData, char *workBuf);

            .export _PrintASCII
            .import popax

            .include "printdrv.inc"
            .include "geossym.inc"

_PrintASCII:
        sta r1L                 ; workBuf -- passed in A/X (fastcall, pointer)
        stx r1H
        jsr popax               ; printData
        sta r0L
        stx r0H
        jsr PrintASCII
        rts
