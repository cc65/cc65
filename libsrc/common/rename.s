;
; Ullrich von Bassewitz, 2003-08-12
;
; int __fastcall__ rename (const char* oldname, const char* newname);
;

        .export         _rename

        .import         __sysrename
        .import         oserrcheck


;--------------------------------------------------------------------------

.proc   _rename

        jsr     __sysrename     ; Call the machine specific function
        jmp     oserrcheck      ; Store into _oserror, set errno, return 0/-1

.endproc




