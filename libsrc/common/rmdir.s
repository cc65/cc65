;
; Oliver Schmidt, 2005-08-30
;
; int __fastcall__ rmdir (const char* name);
;

        .export         _rmdir

        .import         __sysrmdir
        .import         ___mappederrno


;--------------------------------------------------------------------------

.proc   _rmdir

        jsr     __sysrmdir      ; Call the machine specific function
        jmp     ___mappederrno  ; Store into __oserror, set errno, return 0/-1

.endproc
