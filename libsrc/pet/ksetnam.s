;
; Ullrich von Bassewitz, 19.11.2002
;
; SETNAM replacement function for the PETs
;

        .export         SETNAM

        .include        "pet.inc"


.proc   SETNAM
        
        sta     FNLEN
        stx     FNADR
        sty     FNADR+1
        rts

.endproc


