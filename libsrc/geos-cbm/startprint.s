;
; 2026 cc65 project (added for PRINTER driver support, see grc65)
;

; char __fastcall__ StartPrint (char *workBuf);

            .export _StartPrint

            .include "printdrv.inc"
            .include "geossym.inc"

_StartPrint:
        sta r1L                 ; workBuf -- passed in A/X (fastcall, pointer)
        stx r1H
        jsr StartPrint
        txa
        rts
