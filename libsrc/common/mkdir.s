;
; Oliver Schmidt, 2005-08-30
;
; int mkdir (const char* name, ...);	/* May take a mode argument */
;

        .export         _mkdir

        .import         __sysmkdir
        .import         oserrcheck


;--------------------------------------------------------------------------

.proc   _mkdir

        jsr     __sysmkdir      ; Call the machine specific function
        jmp     oserrcheck      ; Store into _oserror, set errno, return 0/-1

.endproc
