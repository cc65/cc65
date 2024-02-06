;
; Ullrich von Bassewitz, 16.11.2002
;
; unsigned char __fastcall__ _sysremove (const char* name);
;

        .export         __sysremove
        .import         fnparse, scratch


;--------------------------------------------------------------------------
; __sysremove:

.proc   __sysremove

        jsr     fnparse         ; Parse the given file name
        bne     err             ; Jump if yes
        jmp     scratch         ; Scratch the file, return an error code
err:    rts

.endproc


