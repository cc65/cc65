;
; Maciej 'YTM/Elysium' Witkowiak
;
; 19.07.2005

; unsigned char __fastcall__ _sysremove (const char* name);

            .export  __sysremove

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"

__sysremove:
        sta r0L
        stx r0H
        jsr DeleteFile
        txa
        ldx #0
        rts
