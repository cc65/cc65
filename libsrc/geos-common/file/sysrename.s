;
; Maciej 'YTM/Elysium' Witkowiak
;
; 19.07.2005

; unsigned char __fastcall__ _sysrename (const char* oldname, const char* newname);

            .export __sysrename
            .import popax

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"

__sysrename:
        sta r0L
        stx r0H
        jsr popax
        sta r6L
        stx r6H
        jsr RenameFile
        txa
        ldx #0
        rts
