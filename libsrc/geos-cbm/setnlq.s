;
; 2026 cc65 project (added for PRINTER driver support, see grc65)
;

; void __fastcall__ SetNLQ (char *workBuf);

            .export _SetNLQ

            .include "printdrv.inc"
            .include "geossym.inc"

_SetNLQ:
        sta r1L                 ; workBuf -- passed in A/X (fastcall, pointer)
        stx r1H
        jmp SetNLQ
