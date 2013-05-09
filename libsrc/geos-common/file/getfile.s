;
; Maciej 'YTM/Elysium' Witkowiak
;
; 4.4.2003

; char __fastcall__ GetFile(char flag, const char *fname, const char *loadaddr, const char *datadname, char *datafname);

            .export _GetFile
            .import popa, popax, setoserror

            .include "jumptab.inc"
            .include "diskdrv.inc"
            .include "geossym.inc"
        
_GetFile:
        sta r3L
        stx r3H
        jsr popax
        sta r2L
        stx r2H
        jsr popax
        sta r7L
        stx r7H
        jsr popax
        sta r6L
        stx r6H
        jsr popa
        sta r0L
        lda #0
        sta r10L
        jsr GetFile
        jmp setoserror
