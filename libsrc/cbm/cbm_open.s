;
; Ullrich von Bassewitz, 22.06.2002
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
;     return _oserror = cbm_k_open();
; }
;

        .export         _cbm_open
        .import         popa
        .import         _cbm_k_setlfs, _cbm_k_setnam, _cbm_k_open
        .import         __oserror

_cbm_open:
        pha
        txa
        pha                     ; Save name

        jsr     popa            ; Get sec_addr
        jsr     _cbm_k_setlfs   ; Call SETLFS, pop all args

        pla
        tax
        pla                     ; Get name
        jsr     _cbm_k_setnam

        jsr     _cbm_k_open
        sta     __oserror

        rts


