;
; 2002-06-22, Ullrich von Bassewitz
; 2021-12-23, Greg King
;
; Original C code by Marc 'BlackJack' Rintsch, 18.03.2001
;
; unsigned char __fastcall__ cbm_open (unsigned char lfn,
;                                      unsigned char device,
;                                      unsigned char sec_addr,
;                                      const char* name);
; /* Opens a file. Works just like the BASIC command.
; ** Returns 0 if opening was successful, otherwise an errorcode (see table
; ** below).
; */
; {
;     cbm_k_setlfs(lfn, device, sec_addr);
;     cbm_k_setnam(name);
;     return __oserror = cbm_k_open();
; }
;

        .export         _cbm_open

        .import         popa
        .import         _cbm_k_setlfs, _cbm_k_setnam, _cbm_k_open
        .import         ___oserror

_cbm_open:
        jsr     _cbm_k_setnam

        jsr     popa            ; Get sec_addr
        jsr     _cbm_k_setlfs   ; Call SETLFS, pop all args

        jsr     _cbm_k_open
        sta     ___oserror
        rts
