;
; Ullrich von Bassewitz, 19.11.2002
;
; READST replacement function for the PETs
;

        .export         READST

        .include        "pet.inc"


.proc   READST

        lda     ST
        rts

.endproc


