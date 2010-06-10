;
; Ullrich von Bassewitz, 2003-08-12
;
; int __fastcall__ chdir (const char* name);
;

        .export         _chdir

        .import         __syschdir
        .import         __mappederrno


;--------------------------------------------------------------------------
; The function calls __syschdir, which must check the directory, set it, and
; copy it to __cwd if it is valid. The copycwd may be used for the latter.

.proc   _chdir

        jsr     __syschdir      ; Call the machine specific function
        jmp     __mappederrno   ; Store into _oserror, set errno, return 0/-1

.endproc


