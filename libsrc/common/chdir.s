;
; Ullrich von Bassewitz, 2003-08-12
;
; int __fastcall__ chdir (const char* name);
;

        .export         _chdir

        .import         __syschdir
        .import         oserrcheck


;--------------------------------------------------------------------------

.proc   _chdir

        jsr     __syschdir      ; Call the machine specific function
        jmp     oserrcheck      ; Store into _oserror, set errno, return 0/-1

.endproc




