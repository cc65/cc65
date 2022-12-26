;
; Ullrich von Bassewitz, 2003-08-12
;
; int __fastcall__ remove (const char* name);
;

        .export         _remove

        .import         __sysremove
        .import         ___mappederrno


;--------------------------------------------------------------------------

.proc   _remove

        jsr     __sysremove     ; Call the machine specific function
        jmp     ___mappederrno  ; Store into __oserror, set errno, return 0/-1

.endproc




