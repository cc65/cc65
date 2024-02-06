;
; Ullrich von Bassewitz, 19.11.2002
;
; OPEN replacement function for the PETs
;

        .export         OPEN
        .import         checkst

        .include        "pet.inc"


.proc   OPEN

        lda     PET_DETECT
        cmp     #PET_4000
        bne     @L1
        jsr     $F563           ; BASIC 4
        jmp     checkst

@L1:    jsr     $F524           ; BASIC 2&3
        jmp     checkst

.endproc


