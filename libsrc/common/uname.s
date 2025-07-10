;
; Ullrich von Bassewitz, 2003-08-12
;
; int __fastcall__ uname (struct utsname* buf);
;

        .export         _uname

        .import         __sysuname
        .import         ___mappederrno


;--------------------------------------------------------------------------

.proc   _uname

        jsr     __sysuname      ; Call the machine specific function
        jmp     ___mappederrno  ; Store into __oserror, set errno, return 0/-1

.endproc


