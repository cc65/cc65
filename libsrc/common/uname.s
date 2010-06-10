;
; Ullrich von Bassewitz, 2003-08-12
;
; int __fastcall__ uname (struct utsname* buf);
;

        .export         _uname

        .import         __sysuname
        .import         __mappederrno


;--------------------------------------------------------------------------

.proc   _uname

        jsr     __sysuname      ; Call the machine specific function
        jmp     __mappederrno   ; Store into _oserror, set errno, return 0/-1

.endproc


