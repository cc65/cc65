;
; Ullrich von Bassewitz, 19.11.2002
;
; RDTIM replacement function for the PETs
;

        .export         RDTIM
        
        .include        "pet.inc"


.proc   RDTIM

        sei
        lda     TIME+2
        ldx     TIME+1
        ldy     TIME
        cli
        rts

.endproc


