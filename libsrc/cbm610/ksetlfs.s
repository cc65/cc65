;
; Ullrich von Bassewitz, 2003-12-18
;
; SETLFS replacement function
;

        .export         SETLFS

        .include        "cbm610.inc"


.proc   SETLFS
        sta     LFN
        stx     DEVNUM
        sty     SECADR
        rts                             ; Return to caller
.endproc


