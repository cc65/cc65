;
; Ullrich von Bassewitz, 19.11.2002
;
; CLOSE replacement function for the PETs
;

        .export         CLOSE

        .include        "pet.inc"


.proc   CLOSE
                     
        ldx     PET_DETECT
        cpx     #PET_4000
        bne     @L1
        jmp     $F2E2           ; BASIC 4
@L1:    jmp     $F2AE           ; BASIC 2&3

.endproc


