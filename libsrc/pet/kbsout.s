;
; Ullrich von Bassewitz, 19.11.2002
;
; BSOUT replacement function for the PETs
;

        .export         BSOUT
        .import         checkst


.proc   BSOUT

        jsr     $FFD2           ; Call kernal function
        jmp     checkst         ; Check status, return carry on error

.endproc


                     
