;
; 2026 cc65 project (added for PRINTER driver support, see grc65)
;

; char __fastcall__ StartASCII (char *workBuf);

            .export _StartASCII

            .include "printdrv.inc"
            .include "geossym.inc"

_StartASCII:
        sta r1L                 ; workBuf -- passed in A/X (fastcall, pointer)
        stx r1H
        jsr StartASCII
        txa
        rts
