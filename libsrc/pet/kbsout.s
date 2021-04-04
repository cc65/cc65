;
; Ullrich von Bassewitz, 19.11.2002
;
; BSOUT/CHROUT replacement function for the PETs
;

        .export         BSOUT
        .export         CHROUT

        .import         checkst


.proc   BSOUT

        jsr     $FFD2           ; Call Kernal function
        jmp     checkst         ; Check status, return carry on error

.endproc

CHROUT  :=      BSOUT
