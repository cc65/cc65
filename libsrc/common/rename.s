;
; Ullrich von Bassewitz, 2003-08-12
;
; int __fastcall__ rename (const char* oldname, const char* newname);
;

        .export         _rename

        .import         __sysrename
        .import         ___mappederrno


;--------------------------------------------------------------------------

.proc   _rename

        jsr     __sysrename     ; Call the machine specific function
        jmp     ___mappederrno   ; Store into __oserror, set errno, return 0/-1

.endproc




