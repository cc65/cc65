;
; Ullrich von Bassewitz, 19.11.2002
;
; SETLFS replacement function for the PETs
;

        .export         SETLFS

        .include        "pet.inc"


.proc   SETLFS

        sta     LFN             ; LFN
        stx     DEVNUM          ; Device address
        sty     SECADR          ; Secondary address
        rts
                      
.endproc


