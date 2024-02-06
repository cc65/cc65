;
; Ullrich von Bassewitz, 22.11.2002
;
; SETLFS replacement function
;

        .export         SETLFS

        .include        "plus4.inc"

; Write directly to the zero page to avoid banking in the ROM

.proc   SETLFS
        sta     LFN
        stx     DEVNUM
        sty     SECADR
        rts                             ; Return to caller
.endproc


