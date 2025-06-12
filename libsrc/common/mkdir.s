;
; Oliver Schmidt, 2005-08-30
;
; int mkdir (const char* name, ...);    /* May take a mode argument */
;

        .export         _mkdir

        .import         __sysmkdir
        .import         ___mappederrno


;--------------------------------------------------------------------------

.proc   _mkdir

        jsr     __sysmkdir      ; Call the machine specific function
        jmp     ___mappederrno  ; Store into __oserror, set errno, return 0/-1

.endproc
